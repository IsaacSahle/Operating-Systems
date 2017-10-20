#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#define NQUEUE 4

struct customer_info{
int user_id;
int service_time;
int arrival_time;
};

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
