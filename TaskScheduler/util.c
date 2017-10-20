#include "airline.h"

void initLocks(){
  // Initialize locks with default attributes
  pthread_mutex_init(&queueLengthLock,NULL);
  pthread_mutex_init(&qLock1,NULL);
  pthread_mutex_init(&qLock2,NULL);
  pthread_mutex_init(&qLock3,NULL);
  pthread_mutex_init(&qLock4,NULL);
  pthread_mutex_init(&clerkLock1,NULL);
  pthread_mutex_init(&clerkLock2,NULL);
}

void initConVars(){
  pthread_cond_init(&qCond1,NULL);
  pthread_cond_init(&qCond2,NULL);
  pthread_cond_init(&qCond3,NULL);
  pthread_cond_init(&qCond4,NULL);
  pthread_cond_init(&clerkCond1,NULL);
  pthread_cond_init(&clerkCond2,NULL);
}
