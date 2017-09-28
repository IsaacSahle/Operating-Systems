#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/fcntl.h>
#include "rsi.h"

void verifyCommand(char * command);
void createProcess(char * token);
void listProcesses();
void sendSignal(char * pid, int signal);
void cleanTerminatedProcesses();
void pstat(char * pid);
void readFile(char * filePath, PstatInfo * stats,int statFlag);
int pidExists(char * pid);

Node * processList;

int main(){
char * command = NULL;
while(TRUE){
  // check for any terminated processes
  command = readline(PMAN_PROMPT);
  cleanTerminatedProcesses();
  verifyCommand(command);
  //readline allocates memory with malloc, need to free
  free(command);
}
return 1;
}

void verifyCommand(char * command){

char * token = strtok(command,COMMAND_DELIMITER);

if(token == NULL){
  //empty string, do nothing
  return;
}else if(strcmp(token,BG) == 0){
  createProcess(token);
}else if(strcmp(token,BGLIST) == 0){
  listProcesses();
}else if(strcmp(token,BGKILL) == 0){
  sendSignal(token,SIGTERM);
}else if(strcmp(token,BGSTOP) == 0){
  sendSignal(token,SIGSTOP);
}else if(strcmp(token,BGSTART) == 0){
  sendSignal(token,SIGCONT);
}else if(strcmp(token,PSTAT) == 0){
  pstat(token);
}else{
  printf("%s%s: %s\n",PMAN_PROMPT,token,UNRECOGNIZED_COMMAND);
}

}

void createProcess(char * token){
  token = strtok(NULL,COMMAND_DELIMITER);
  struct stat fileStats;
  int result = stat(token, &fileStats);
  if(token == NULL || result != 0){
    fprintf(stdout,"%s%s\n",PMAN_PROMPT,BG_INCORRECT_USAGE);
    return;
  }

  pid_t processId = fork();
  if(processId < 0){
    fprintf(stderr,"%s%s\n",PMAN_PROMPT,FORK_ERROR_MESSAGE);
  }else if(processId == 0){
    //child process
    char * programArguments [MAX_ARGUMENTS];
    int i = 0;
    while(token != NULL && i < MAX_ARGUMENTS){
      programArguments[i] = malloc(strlen(token) + 1);
      strcpy(programArguments[i],token);
      token = strtok(NULL,COMMAND_DELIMITER);
      i++;
    }
     programArguments[i] = NULL;

    if(execvp(programArguments[0],programArguments) == -1){
      for(i = 0; i < MAX_ARGUMENTS;i++){
       if(programArguments[i] != NULL){
        free(programArguments[i]);
       }
      }
      fprintf(stderr,"%s%s\n",PMAN_PROMPT,EXECVP_ERROR_MESSAGE);
      exit(EXIT_FAILURE);
    }
   }else{
    insertNode(&processList,processId);
   }
}

void listProcesses(){
  Node * curr = processList;
  int count = 0;
  while(curr != NULL){
    char procSymlink[MAX_PATH_SIZE + strlen(PROC_PREFIX) + strlen(EXE_SUFFIX)];
    char procPath[MAX_PATH_SIZE];
    int numCopiedBytes = snprintf(procSymlink,sizeof procSymlink,"%s%ld%s",PROC_PREFIX,(long)curr->processId,EXE_SUFFIX);
    if(numCopiedBytes > 0 && numCopiedBytes < sizeof procSymlink){
      ssize_t numBytes = readlink(procSymlink,procPath,MAX_PATH_SIZE);
      if(numBytes == -1){
        fprintf(stderr,"%s\n",READLINK_ERROR_MESSAGE);
      }
      procPath[numBytes] = '\0';
      fprintf(stdout,"%ld: %s\n",(long)curr->processId,procPath);
    }

    curr = curr->next;
    count++;
  }
  fprintf(stdout,"%s: %d\n",TOTAL_JOBS,count);
}

void sendSignal(char * pid, int signal){
  pid = strtok(NULL,COMMAND_DELIMITER);
  if(pid == NULL){
    fprintf(stdout,"%s%s\n",PMAN_PROMPT,BGKILL_INCORRECT_USAGE);
    return;
  }
  pid_t processId = strtol(pid,NULL,BASE_TEN);
  int success = kill(processId,signal);
  if(success == -1){
    fprintf(stderr,"%s%s\n",PMAN_PROMPT,KILL_ERROR_MESSAGE);
  }
}

void cleanTerminatedProcesses(){
  pid_t childPid;
  while((childPid = waitpid(ANY_CHILD,NULL,WNOHANG)) > 0){
    removeNode(&processList,childPid);
    fprintf(stdout,"%ld: %s\n",(long) childPid,PROCESS_TERMINATED);
  }
}

void pstat(char * pid){
  pid = strtok(NULL,COMMAND_DELIMITER);
  if(pid == NULL){
    fprintf(stdout,"%s%s\n",PMAN_PROMPT,PSTAT_INCORRECT_USAGE);
    return;
  }else if(!pidExists(pid)){
    fprintf(stdout,"%s%s %s %s\n",PMAN_PROMPT,PID_PREFIX_ERROR_MESSAGE,pid,PID_SUFFIX_ERROR_MESSAGE);
    return; 
  }

  char procStatPath[MAX_PATH_SIZE + strlen(PROC_PREFIX) + strlen(STAT_SUFFIX)];
  char procStatusPath[MAX_PATH_SIZE + strlen(PROC_PREFIX) + strlen(STATUS_SUFFIX) + strlen(GREP_PREFIX)];
  PstatInfo stats;
  snprintf(procStatPath, sizeof procStatPath, "%s%s%s",PROC_PREFIX,pid,STAT_SUFFIX);
  snprintf(procStatusPath, sizeof procStatusPath, "%s %s%s%s",GREP_PREFIX,PROC_PREFIX,pid,STATUS_SUFFIX);
  readFile(procStatPath,&stats,1);
  readFile(procStatusPath,&stats,0);
  fprintf(stdout, "%s\n%c\n%.4f\n%.4f\n%ld\n%s%s",stats.name,stats.state,(float)stats.utime/sysconf(_SC_CLK_TCK),(float)stats.stime/sysconf(_SC_CLK_TCK),stats.rss,stats.voluntaryContextSwitch,stats.nonvoluntaryContextSwitch);
}

void readFile(char * filePath, PstatInfo * stats,int statFlag){
  FILE * fp = statFlag?fopen(filePath,READ) : popen(filePath,READ);
  if(fp == NULL){
    fprintf(stderr,"%s%s\n",PMAN_PROMPT,STATUS_READ_ERROR);
    return;
  }else if(statFlag){
    fscanf(fp,"%*d %s %c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %*d %*d %*d %*d %*d %*d %*u %*u %ld",stats->name,&(stats->state),&(stats->utime),&(stats->stime),&(stats->rss));
    fclose(fp);
  }else{
    fgets(stats->voluntaryContextSwitch, sizeof (stats->voluntaryContextSwitch),fp);
    fgets(stats->nonvoluntaryContextSwitch, sizeof (stats->nonvoluntaryContextSwitch),fp);
    pclose(fp);
  }
}

int pidExists(char * pid){
 pid_t processId = strtol(pid,NULL,BASE_TEN);
 return searchList(&processList,processId); 
}
