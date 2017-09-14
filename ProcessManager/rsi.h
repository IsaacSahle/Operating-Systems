#ifndef RSI_HEADER_FILE
#include <unistd.h>
#include <stdlib.h>
#define RSI_HEADER_FILE

/* Integer Constants */
#define TRUE 1
#define MAX_ARGUMENTS 25
#define MAX_PATH_SIZE 100
#define BASE_TEN 10
#define ANY_CHILD -1
#define STOPPED 0

/* String Constants */
#define SYMLINK_PREFIX "/proc/"
#define SYMLINK_SUFFIX "/exe"
#define TOTAL_JOBS "Total background jobs"
#define PMAN_PROMPT "PMan: > "
#define COMMAND_DELIMITER " "
#define PROCESS_TERMINATED "Process terminated"

/* Incorrect Usage */
#define BG_INCORRECT_USAGE "Usage: bg <required: file name> <optional: arguments>"
#define BGKILL_INCORRECT_USAGE "Usage: bgkill <required: pid>"
#define UNRECOGNIZED_COMMAND "command not found"

/* Error Messages */
#define EXECVP_ERROR_MESSAGE "ERROR: execvp failed"
#define FORK_ERROR_MESSAGE "ERROR: forking failed"
#define READLINK_ERROR_MESSAGE "ERROR: readlink failed to read any bytes"
#define KILL_ERROR_MESSAGE "ERROR: call to kill failed"

/* Commands */
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
void sortList(Node ** head);

#endif
