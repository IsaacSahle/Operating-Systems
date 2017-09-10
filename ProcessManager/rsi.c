#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "rsi.h"

void verifyCommand(char * command);
void createProcess(char * token);
void listProcesses();

Node * processList;

int main(){
char * command = NULL;
while(TRUE){
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
  printf("%s\n",token); 
}else if(strcmp(token,BGKILL) == 0){
  printf("%s\n",token); 
}else if(strcmp(token,BGSTOP) == 0){
  printf("%s\n",token); 
}else if(strcmp(token,BGSTART) == 0){
  printf("%s\n",token);
}else if(strcmp(token,PSTAT) == 0){
  printf("%s\n",token); 
}else{ 
  printf("%s%s: %s\n",PMAN_PROMPT,token,UNRECOGNIZED_COMMAND); 
}

}

void createProcess(char * token){
 
  pid_t processId = fork();
  if(processId < 0){
    fprintf(stderr,"%s%s\n",PMAN_PROMPT,FORK_ERROR_MESSAGE);
  }else if(processId == 0){
    //child process
    token = strtok(NULL,COMMAND_DELIMITER);
    if(token == NULL){
     fprintf(stdout,"%s%s\n",PMAN_PROMPT,BG_INCORRECT_USAGE_MESSAGE);
     exit(EXIT_FAILURE);
    }
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
    //record the creation of child process
    insertNode(&processList,processId); 
   }
}

void listProcesses(){
  //sort processlist by pid
  //retrieve path from \proc
}







