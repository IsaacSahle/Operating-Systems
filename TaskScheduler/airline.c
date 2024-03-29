#include "airline.h"

void * customer_runner(void * info);
void * clerk_runner(void * info);

int main(int argc, char * argv []) {
  FILE * fp;
  int fileOpen = (argc == 2 && (fp = fopen(argv[1],READ)) != NULL);
  srand(time(NULL)); 
  if(!fileOpen){
    fprintf(stderr,"%s\n",ERROR_INPUT_FILE);
    return EXIT_FAILURE;
  } 
  
  initLocks();
  initConVars();
  gettimeofday(&init_time, NULL); 
  memset(queue_lengths,0,sizeof(queue_lengths));
  memset(serve_customer_ids,DEFAULT_CU_ID,sizeof(NCLERKS));

  char line_buffer[MAX_BUFFER];
  memset(line_buffer,0,sizeof(line_buffer));
  fgets(line_buffer, MAX_BUFFER,fp); 
  
  int num_customers = line_buffer == NULL ? 0 : atoi(line_buffer);
  // TODO(isaacsahle): Optimization point, waste less memory by determining the number of valid customers ahead of time
  pthread_t customers [num_customers];
  customer_info cus_info [num_customers];
  int valid_customers [num_customers];
 
  pthread_t clerks [NCLERKS];
  clerk_info cl_info [NCLERKS];
  char * token;
  
  int i;
  // Spawn clerk threads
  for(i = 0; i < NCLERKS; i++){
    cl_info[i].clerk_num = (i);
    pthread_create(&clerks[i],NULL,clerk_runner,(void *) &cl_info[i]);
  } 
 
  // Spawn customer threads
  for(i = 0; i < num_customers;i++){
    fgets(line_buffer, MAX_BUFFER,fp);
    token = strtok(line_buffer,DELIMITERS);
    cus_info[i].customer_id = atoi(token);
    token = strtok(NULL,DELIMITERS);
    cus_info[i].arrival_time = atoi(token);
    token = strtok(NULL,DELIMITERS);
    cus_info[i].service_time = atoi(token);
    if(cus_info[i].customer_id >= 0 && cus_info[i].arrival_time >= 0 && cus_info[i].service_time >= 0){ 
     int isSuccess = pthread_create(&customers[i],NULL,customer_runner,(void *) &cus_info[i]); 
     valid_customers[i] = (isSuccess == 0) ? VALID_CU : INVALID_CU;
    }else{
      valid_customers[i] = INVALID_CU;
    }
  }
  
  fclose(fp);
 
  // Join on valid customer threads(non-neg,pthread_create() success)
  int num_invalid_cu = 0;
  for(i = 0; i < num_customers; i++){
    if(valid_customers[i] == VALID_CU){ 
      pthread_join(customers[i],NULL);
    }else{
      num_invalid_cu++;
    }
  }

  fprintf(stdout,"%s %.4f %s\n",AVG_WAIT_TIME_PRE,overall_waiting_time/(num_customers - num_invalid_cu),AVG_WAIT_TIME_SUFF);
  
  return 0;
}

void * customer_runner(void * info){
  customer_info *  customer_inf = (customer_info *) info;
  // Arrival time is a 10th of a second (e.g time == 2 is .2 seconds)
  usleep(customer_inf->arrival_time * MICROSECOND_CONVERSION);
  pthread_mutex_lock(&init_time_lock);
  fprintf(stdout, "%s %d %s: %.2f.\n",CU_ARRIVES,customer_inf->customer_id,AT_TIME,getCurrentSimulationTime(&init_time));
  pthread_mutex_unlock(&init_time_lock);  

  int queue_num;
  int q_length;
  int my_clerk_num;

 // Determine shortest queue  
  pthread_mutex_lock(&q_lock);
  pthread_mutex_lock(&queue_lengths_lock);
  queue_num = retrieveQueueNumber(SHORTEST);

  enqueue(&queues[queue_num],customer_inf->customer_id);
  queue_lengths[queue_num]++;
  q_length = queue_lengths[queue_num]; 

  pthread_mutex_unlock(&queue_lengths_lock);
  
  // Start calculating wait time
  struct timeval queue_time;
  gettimeofday(&queue_time,NULL);
 
  pthread_mutex_lock(&init_time_lock);
  fprintf(stdout, "%s %d %s %d %s: %.2f\n",CU_ENTERS_PRE,(queue_num + 1),CU_ENTERS_SUFF,q_length,AT_TIME,getCurrentSimulationTime(&init_time));
  pthread_mutex_unlock(&init_time_lock); 
 

  // Wake any idle clerks threads
  pthread_mutex_lock(&empty_queues_lock);
  pthread_cond_signal(&empty_queues_cond);
  pthread_mutex_unlock(&empty_queues_lock);
 
  // Wait on queue condition variable
  while(1){
    pthread_cond_wait(&q_conds[queue_num],&q_lock);
    
    if(customer_inf->customer_id == peek(&queues[queue_num])){
       // Sweet, we are at the front of queue & clerk has selected us :)
      updateTotalWaitTime(&queue_time);
      
      pthread_mutex_lock(&queue_lengths_lock);
      queue_lengths[queue_num]--;
      dequeue(&queues[queue_num]);
      pthread_mutex_lock(&clerk_serving_lock);
      
      int i;
      my_clerk_num = -1;
      for(i = 0; i < NCLERKS;i++){
        if(customer_inf->customer_id  == serve_customer_ids[i]){
          serve_customer_ids[i] = DEFAULT_CU_ID;
          my_clerk_num = i; 
        }
      }

      pthread_mutex_unlock(&clerk_serving_lock);
      pthread_mutex_unlock(&queue_lengths_lock);
      pthread_mutex_unlock(&q_lock);   
      break;
    }

    pthread_cond_signal(&q_conds[queue_num]); 
  }

  pthread_mutex_lock(&init_time_lock); 
  fprintf(stdout,"%s %s %s %s %.2f %s %d %s %d.\n",CL,START,CL_SERVE,START_TIME,getCurrentSimulationTime(&init_time),CU_ID,customer_inf->customer_id,CL_ID,(my_clerk_num + 1));
  pthread_mutex_unlock(&init_time_lock); 
  // Service time is a 10th of a second 
  usleep(customer_inf->service_time * MICROSECOND_CONVERSION);
  pthread_mutex_lock(&init_time_lock);
  fprintf(stdout,"%s %s %s %s %.2f %s %d %s %d.\n",CL,FINISHES,CL_SERVE,END_TIME,getCurrentSimulationTime(&init_time),CU_ID,customer_inf->customer_id,CL_ID,(my_clerk_num + 1));
  pthread_mutex_unlock(&init_time_lock); 

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
    
   // Sleep until a customer signals they entered a queue
    if(queue_length == 0){
      pthread_mutex_lock(&empty_queues_lock);
      pthread_cond_wait(&empty_queues_cond,&empty_queues_lock);
      pthread_mutex_unlock(&empty_queues_lock);
      continue;
    }
    
    // Signal to customer in longest queue
    pthread_mutex_lock(&q_lock);
    pthread_mutex_lock(&clerk_serving_lock);
    
    // Write customer id for verifying clerk in customer thread
    // Check if other clerk is serving this customer
    serve_customer_ids[clerk_inf->clerk_num] = peek(&queues[queue_index]);
    if(isBeingServed(clerk_inf->clerk_num)){
      serve_customer_ids[clerk_inf->clerk_num] = DEFAULT_CU_ID;
      pthread_mutex_unlock(&q_lock);
      pthread_mutex_unlock(&clerk_serving_lock);
      continue;
    }
    
    pthread_cond_signal(&q_conds[queue_index]); 
    pthread_mutex_unlock(&clerk_serving_lock);
    pthread_mutex_unlock(&q_lock);
  
    // Wait for customer thread to signal back once service is finished 
    pthread_mutex_lock(&clerk_locks[clerk_inf->clerk_num]);
    pthread_cond_wait(&clerk_conds[clerk_inf->clerk_num],&clerk_locks[clerk_inf->clerk_num]);
    pthread_mutex_unlock(&clerk_locks[clerk_inf->clerk_num]);
  }

  pthread_exit(NULL);
  return NULL;
}
