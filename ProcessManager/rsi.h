#ifndef RSI_HEADER_FILE
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#define RSI_HEADER_FILE

/* Integer Constants */
#define TRUE 1
#define MAX_ARGUMENTS 25
#define MAX_PATH_SIZE 100
#define BASE_TEN 10
#define ANY_CHILD -1
#define STOPPED 0
#define MAX_PSTAT 255
#define SLEEP_TIME 1000

/* String Constants */
#define PROC_PREFIX "/proc/"
#define EXE_SUFFIX "/exe"
#define STAT_SUFFIX "/stat"
#define STATUS_SUFFIX "/status"
#define TOTAL_JOBS "Total background jobs"
#define PMAN_PROMPT "PMan: > "
#define COMMAND_DELIMITER " "
#define PROCESS_TERMINATED "Process terminated"
#define READ "r"
#define GREP_PREFIX "grep ctxt"
#define PROCESS_STATE "grep State:"
#define PMAN_QUIT "quit"

/* Incorrect Usage */
#define BG_INCORRECT_USAGE "Usage: bg <required: file name> <optional: arguments>"
#define BGKILL_INCORRECT_USAGE "Usage: bgkill <required: pid>"
#define PSTAT_INCORRECT_USAGE "Usage: pstat <required: pid>"
#define UNRECOGNIZED_COMMAND "command not found"

/* Error Messages */
#define EXECVP_ERROR_MESSAGE "ERROR: execvp failed"
#define FORK_ERROR_MESSAGE "ERROR: forking failed"
#define READLINK_ERROR_MESSAGE "ERROR: readlink failed to read any bytes"
#define KILL_ERROR_MESSAGE "ERROR: call to kill failed"
#define STATUS_READ_ERROR "ERROR: failed to read one of /proc/pid/stat or /proc/pid/status"
#define PID_PREFIX_ERROR_MESSAGE "ERROR: Process"
#define PID_SUFFIX_ERROR_MESSAGE "doesn't exist"

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

typedef struct {
  char name[MAX_PSTAT];
  char state;
  unsigned long int utime;
  unsigned long int stime;
  long int rss;
  char voluntaryContextSwitch[MAX_PSTAT];
  char nonvoluntaryContextSwitch[MAX_PSTAT];
} PstatInfo;

void insertNode(Node ** head, pid_t pid);
void removeNode(Node ** head, pid_t pid);
int size(Node * head);
void sortList(Node ** head);
int searchList(Node ** head, pid_t pid);
void removeAll(Node ** head);
#endif
