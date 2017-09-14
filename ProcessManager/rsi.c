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

Node * processList;

int main(){
char * command = NULL;
while(TRUE){
  // check for any terminated processes
  command = readline(PMAN_PROMPT);
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
  printf("%s\n",token); 
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
    char procSymlink[MAX_PATH_SIZE + strlen(SYMLINK_PREFIX) + strlen(SYMLINK_SUFFIX)];
    char procPath[MAX_PATH_SIZE];
    int numCopiedBytes = snprintf(procSymlink,sizeof procSymlink,"%s%ld%s",SYMLINK_PREFIX,(long)curr->processId,SYMLINK_SUFFIX);
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
