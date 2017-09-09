#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "rsi.h"

void verifyCommand(char * command);

int main(){
char * command = NULL;
//readline allocates memory with malloc, remember to free
command = readline(PMAN_PROMPT);

while(TRUE){
  verifyCommand(command);
  free(command);
  command = readline(PMAN_PROMPT);
}

return 1;
}

void verifyCommand(char * command){
//identify command
char * token = strtok(command,COMMAND_DELIMITER);

if(strcmp(token,BG) == 0){
  printf("%s\n",token); 
}else if(strcmp(token,BGLIST) == 0){
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
  printf("%s\n",UNRECOGNIZED_COMMAND); 
}

}
