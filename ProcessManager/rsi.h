#ifndef RSI_HEADER_FILE
#include <unistd.h>
#include <stdlib.h>
#define RSI_HEADER_FILE
#define TRUE 1
#define MAX_ARGUMENTS 25
#define PMAN_PROMPT "PMan: > "
#define COMMAND_DELIMITER " "
#define BG_INCORRECT_USAGE_MESSAGE "Incorrect usage: bg <required: file name> <optional: arguments>"
#define UNRECOGNIZED_COMMAND "command not found"
#define EXECVP_ERROR_MESSAGE "ERROR: execvp failed"
#define FORK_ERROR_MESSAGE "ERROR: forking failed"
#define BG "bg"
#define BGLIST "bglist"
#define BGKILL "bgkill"
#define BGSTOP "bgstop"
#define BGSTART "bgstart" 
#define PSTAT "pstat"

typedef struct Node{
  pid_t processId;
  struct Node * next;
} Node;

void insertNode(Node ** head, pid_t pid);
void removeNode(Node ** head, pid_t pid);
int size(Node * head);

#endif
