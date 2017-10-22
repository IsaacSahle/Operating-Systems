#include "airline.h"

void * customer_runner(void * info);

struct timeval init_time;
double overall_waiting_time;
int queue_length[NQUEUE];
 
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

  char lineBuffer[MAX_BUFFER];
  memset(lineBuffer,0,sizeof(lineBuffer));
  fgets(lineBuffer, MAX_BUFFER,fp); 
  
  int numCustomers = lineBuffer == NULL ? 0 : atoi(lineBuffer);
  pthread_t customers [numCustomers];
  customer_info info [numCustomers];
  char * token;
  
  int i;
  for(i = 0; i < numCustomers;i++){
    // initialize info
    fgets(lineBuffer, MAX_BUFFER,fp);
    token = strtok(lineBuffer,DELIMITERS);
    info[i].customer_id = atoi(token);
    token = strtok(NULL,DELIMITERS);
    info[i].arrival_time = atoi(token);
    token = strtok(NULL,DELIMITERS);
    info[i].service_time = atoi(token);
    pthread_create(&customers[i],NULL,customer_runner,(void *) &info[i]);
  }
  
  for(i = 0; i < numCustomers; i++){
     pthread_join(customers[i],NULL);
  }
  
  return 0;
}





void * customer_runner(void * info){
  customer_info *  customerInfo = (customer_info *) info;
  // Arrival time is a 10th of a second (e.g time == 2 is .2 seconds)
  usleep(customerInfo->arrival_time * MICROSECOND_CONVERSION);
  fprintf(stdout, "A customer arrives: customer ID %d. \n", customerInfo->customer_id);

  //printf("%d\n",customerInfo->customer_id);
  pthread_exit(NULL);
  return NULL;
}


