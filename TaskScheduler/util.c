#include "airline.h"

void initLocks(){
  int i;
  for(i = 0; i < NQUEUE; i++){
    pthread_mutex_init(&q_locks[i],NULL);
  }
  for(i = 0; i < NCLERKS; i++){
    pthread_mutex_init(&clerk_locks[i],NULL);
    pthread_mutex_init(&clerk_serving_locks[i],NULL);
  } 
  pthread_mutex_init(&queue_lengths_lock,NULL);
  pthread_mutex_init(&empty_queues_lock,NULL);
  pthread_mutex_init(&total_wait_time_lock,NULL);
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

// Retrieves index of the first (queues of equal length) shortest or longest queue
int retrieveQueueNumber(int flag){
 int i;
 int queue_num = queue_lengths[0];
 int index = 0;
 for(i = 0; i < NQUEUE;i++){
   if(queue_lengths[i] < queue_num && flag == SHORTEST){
     queue_num = queue_lengths[i];
     index = i;
   }else if(queue_lengths[i] > queue_num && flag == LONGEST){
     queue_num = queue_lengths[i];
     index = i;
   }
 }
 return index;
}

int retrieveClerk(int c_id){
  int i;
  for(i = 0; i < NCLERKS;i++){
    pthread_mutex_lock(&clerk_serving_locks[i]);
    if(c_id == serve_customer_ids[i]){
      serve_customer_ids[i] = DEFAULT_CU_ID;
      pthread_mutex_unlock(&clerk_serving_locks[i]);
      return i;
    }
   pthread_mutex_unlock(&clerk_serving_locks[i]);
  }
  return -1;
}

int isValidNextCustomer(int queue_num,int c_id){
 return c_id == peek(&queues[queue_num]);
}

void updateTotalWaitTime(struct timeval * queue_time){
  double wait_time = getCurrentSimulationTime(queue_time);
  pthread_mutex_lock(&total_wait_time_lock);
  overall_waiting_time += wait_time; 
  pthread_mutex_unlock(&total_wait_time_lock);
}

void handleClerkSelection(int queue_num,int customer_id,int * cl_num){
  pthread_mutex_lock(&queue_lengths_lock);
  queue_lengths[queue_num]--;
  dequeue(&queues[queue_num]);
  (*cl_num) = retrieveClerk(customer_id);
  pthread_mutex_unlock(&queue_lengths_lock);
  pthread_mutex_unlock(&q_locks[queue_num]);   
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
  if(head == NULL){
    return -1;
  }
  
  node_t * current = *head;
  while(current->next != NULL){
    current = current->next;
  }
  return current->customer_id;
}

void print_list(node_t *head) {
    node_t *current = head;

    while (current != NULL) {
        printf("%d\n", current->customer_id);
        current = current->next;
    }
}
