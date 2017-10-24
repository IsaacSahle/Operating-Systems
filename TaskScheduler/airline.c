#include "airline.h"

void * customer_runner(void * info);
void * clerk_runner(void * info);

struct timeval init_time;
double overall_waiting_time;

int main(int argc, char * argv []) {
  FILE * fp;
  int fileOpen = (argc == 2 && (fp = fopen(argv[1],READ)) != NULL);
  
  if(!fileOpen){
    fprintf(stderr,"%s\n",ERROR_INPUT_FILE);
    return EXIT_FAILURE;
  } 
  
  // initialize all the condition variable and thread lock will be used
  initLocks();
  initConVars();
  memset(queue_lengths,0,sizeof(queue_lengths));

  char line_buffer[MAX_BUFFER];
  memset(line_buffer,0,sizeof(line_buffer));
  fgets(line_buffer, MAX_BUFFER,fp); 
  
  int num_customers = line_buffer == NULL ? 0 : atoi(line_buffer);
  pthread_t customers [num_customers];
  customer_info cus_info [num_customers];
  pthread_t clerks [NCLERKS];
  clerk_info cl_info [NCLERKS];
  char * token;
  
  int i;
  for(i = 0; i < NCLERKS; i++){
    cl_info[i].clerk_num = (i);
    pthread_create(&clerks[i],NULL,clerk_runner,(void *) &cl_info[i]);
  } 
 
  for(i = 0; i < num_customers;i++){
    // initialize info
    fgets(line_buffer, MAX_BUFFER,fp);
    token = strtok(line_buffer,DELIMITERS);
    cus_info[i].customer_id = atoi(token);
    token = strtok(NULL,DELIMITERS);
    cus_info[i].arrival_time = atoi(token);
    token = strtok(NULL,DELIMITERS);
    cus_info[i].service_time = atoi(token);
    pthread_create(&customers[i],NULL,customer_runner,(void *) &cus_info[i]);
  }
  
  for(i = 0; i < num_customers; i++){
     pthread_join(customers[i],NULL);
  }
  
  return 0;
}

void * customer_runner(void * info){
  customer_info *  customer_inf = (customer_info *) info;
  // Arrival time is a 10th of a second (e.g time == 2 is .2 seconds)
  usleep(customer_inf->arrival_time * MICROSECOND_CONVERSION);
  fprintf(stdout, "A customer arrives: customer ID %d. \n", customer_inf->customer_id);

  int queue_num;
  int q_length;
  int my_clerk_num;

 // Determine shortest queue  
  pthread_mutex_lock(&queue_lengths_lock);
  queue_num = retrieveQueueNumber(SHORTEST);
  pthread_mutex_lock(&q_locks[queue_num]);  

  enqueue(&queues[queue_num],customer_inf->customer_id);
  queue_lengths[queue_num]++;
  q_length = queue_lengths[queue_num]; 

  pthread_mutex_unlock(&queue_lengths_lock);
  
  fprintf(stdout, "A customer enters a queue: the queue ID %1d, and length of the queue %d. \n",queue_num,q_length);
  
  // Wake any idle clerks threads
  pthread_mutex_lock(&empty_queues_lock);
  pthread_cond_broadcast(&empty_queues_cond);
  pthread_mutex_unlock(&empty_queues_lock);
  
  // Wait on queue condition variable
  while(1){
    pthread_cond_wait(&q_conds[queue_num],&q_locks[queue_num]);
    if(isValidNextCustomer(queue_num,customer_inf->customer_id)){
       // Sweet, we are at the front of queue & clerk has selected us :)
       pthread_mutex_lock(&queue_lengths_lock);
       pthread_mutex_lock(&clerk_serving_lock);
       queue_lengths[queue_num]--;
       dequeue(&queues[queue_num]);
       my_clerk_num = retrieveClerk(customer_inf->customer_id);
       pthread_mutex_unlock(&clerk_serving_lock);
       pthread_mutex_unlock(&queue_lengths_lock);
       pthread_mutex_unlock(&q_locks[queue_num]);  
       break;
    } 
    pthread_cond_signal(&q_conds[queue_num]);
  }
   
  fprintf(stdout,"Customer: %d starts being served by %d\n",customer_inf->customer_id,my_clerk_num);
  // Service time is a 10th of a second 
  usleep(customer_inf->service_time * MICROSECOND_CONVERSION);
  fprintf(stdout,"Customer: %d finished being served by %d\n",customer_inf->customer_id,my_clerk_num);
  

  // Notify clerk we are done
  pthread_mutex_lock(&clerk_locks[my_clerk_num]);
  pthread_cond_signal(&clerk_conds[my_clerk_num]);
  pthread_mutex_unlock(&clerk_locks[my_clerk_num]);
  
  pthread_exit(NULL);
  return NULL;
}

void * clerk_runner(void * info){
  clerk_info* clerk_inf = (clerk_info*) info;

  while(1){
    // Determine longest queue
    int queue_index;
    int queue_length;
    pthread_mutex_lock(&queue_lengths_lock);
    queue_index = retrieveQueueNumber(LONGEST);
    queue_length = queue_lengths[queue_index];
    pthread_mutex_unlock(&queue_lengths_lock);
    
    // When all queues are empty sleep until a customer signals they entered a queue
    if(queue_length == 0){
      pthread_mutex_lock(&empty_queues_lock);
      pthread_cond_wait(&empty_queues_cond,&empty_queues_lock);
      pthread_mutex_unlock(&empty_queues_lock);
      continue;
    }

    // Signal to customer in longest queue
    pthread_mutex_lock(&q_locks[queue_index]);
    pthread_mutex_lock(&clerk_serving_lock);
    // Write customer id for verifying clerk in customer thread
    serve_customer_ids[clerk_inf->clerk_num] = peek(&queues[queue_index]);
    pthread_cond_signal(&q_conds[queue_index]); 
    pthread_mutex_unlock(&clerk_serving_lock);
    pthread_mutex_unlock(&q_locks[queue_index]);
  
    // Wait for customer thread to signal back once service is finished
    pthread_mutex_lock(&clerk_locks[clerk_inf->clerk_num]);
    pthread_cond_wait(&clerk_conds[clerk_inf->clerk_num],&clerk_locks[clerk_inf->clerk_num]);
    pthread_mutex_unlock(&clerk_locks[clerk_inf->clerk_num]);
  }

  pthread_exit(NULL);
  return NULL;
}
