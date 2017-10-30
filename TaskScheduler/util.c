#include "airline.h"

void initLocks(){
  int i;
  for(i = 0; i < NCLERKS; i++){
    pthread_mutex_init(&clerk_locks[i],NULL);
  } 
  pthread_mutex_init(&queue_lengths_lock,NULL);
  pthread_mutex_init(&empty_queues_lock,NULL);
  pthread_mutex_init(&total_wait_time_lock,NULL);
  pthread_mutex_init(&clerk_serving_lock,NULL);
  pthread_mutex_init(&init_time_lock,NULL);
  pthread_mutex_init(&q_lock,NULL);
}

void initConVars(){
  int i;
  for(i = 0; i < NQUEUE; i++){
    pthread_cond_init(&q_conds[i],NULL);
  }
  for(i = 0; i < NCLERKS; i++){
    pthread_cond_init(&clerk_conds[i],NULL);
  }
  pthread_cond_init(&empty_queues_cond,NULL);
}

// Retrieves index of queue with shortest or longest length
// TODO(isaacsahle): this can be done better, come back if time
int retrieveQueueNumber(int flag){
 int i;
 int valid_indexes[NQUEUE];
 memset(valid_indexes,-1,sizeof(valid_indexes));
 int queue_num = queue_lengths[0];
 int index = 0;
 
  // Determine min or max value
  for(i = 0; i < NQUEUE;i++){
   if(queue_lengths[i] < queue_num && flag == SHORTEST){
     queue_num = queue_lengths[i];
   }else if(queue_lengths[i] > queue_num && flag == LONGEST){
     queue_num = queue_lengths[i];
   }
 }
 
 // Determine all queues with min or max value length
 int sameLengthCount = 0;
 for(i = 0; i < NQUEUE;i++){
   if(queue_lengths[i] == queue_num){
     valid_indexes[i] = 0;
     sameLengthCount++;
   }
 }

 // Select random queue of length min or max
 int r_q_num = 1 + rand() % sameLengthCount;
 int count = 1;
 for(i = 0; i < NQUEUE;i++){
   if(valid_indexes[i] == 0){
     if(count == r_q_num){
        return i;
     }
     count++; 
   }
 }

 return index;
}

int isBeingServed(int cl_index){
   int i;
   for(i = 0; i < NCLERKS;i++){
     if(serve_customer_ids[cl_index] == serve_customer_ids[i] && i != cl_index){
       return 1;
     
     }
   }
   return 0;
}

void updateTotalWaitTime(struct timeval * queue_time){
  double wait_time = getCurrentSimulationTime(queue_time);
  pthread_mutex_lock(&total_wait_time_lock);
  overall_waiting_time += wait_time; 
  pthread_mutex_unlock(&total_wait_time_lock);
}

//Source: connex assignment #2 resources (sample_gettimeofday.c)
double getCurrentSimulationTime(struct timeval * start_time){
  
  struct timeval cur_time;
  double cur_secs, init_secs;
  
  init_secs = (start_time->tv_sec + (double) start_time->tv_usec / 1000000);
  
  gettimeofday(&cur_time, NULL);
  cur_secs = (cur_time.tv_sec + (double) cur_time.tv_usec / 1000000);
  
  return cur_secs - init_secs;
}

// Source for queue operations: https://gist.github.com/kroggen/5fc7380d30615b2e70fcf9c7b69997b6
void enqueue(node_t **head, int val) {
    node_t *new_node = malloc(sizeof(node_t));
    if (!new_node) return;

    new_node->customer_id = val;
    new_node->next = *head;

    *head = new_node;
}

// TODO(isaac): Optimization point, don't traverese the 
// whole queue, keep tail pointer and remove in O(1).
int dequeue(node_t **head) {
    node_t *current, *prev = NULL;
    int retval = -1;

    if (*head == NULL) return -1;

    current = *head;
    while (current->next != NULL) {
        prev = current;
        current = current->next;
    }

    retval = current->customer_id;
    free(current);
    
    if (prev)
        prev->next = NULL;
    else
        *head = NULL;

    return retval;
}

int peek(node_t ** head){
  if(*head == NULL){
    return -1;
  }
  
  node_t * current = *head;
  while(current->next != NULL){
    current = current->next;
  }
  return current->customer_id;
}
