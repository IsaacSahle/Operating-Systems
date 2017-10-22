#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Integer Constants
#define NQUEUE 4
#define MAX_BUFFER 10
#define MICROSECOND_CONVERSION 100000

//String Constants
#define READ "r"
#define DELIMITERS ":,"
#define ERROR_INPUT_FILE "ERROR: incorrect num args or cannot open input file"

typedef struct {
int customer_id;
int service_time;
int arrival_time;
} customer_info;

pthread_mutex_t queueLengthLock;
pthread_mutex_t qLock1;
pthread_mutex_t qLock2;
pthread_mutex_t qLock3;
pthread_mutex_t qLock4;
pthread_mutex_t clerkLock1;
pthread_mutex_t clerkLock2;
pthread_cond_t qCond1;
pthread_cond_t qCond2;
pthread_cond_t qCond3;
pthread_cond_t qCond4;
pthread_cond_t clerkCond1;
pthread_cond_t clerkCond2;

void initLocks();
void initConVars();
