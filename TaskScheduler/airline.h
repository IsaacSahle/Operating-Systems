#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>

// Integer Constants
#define NQUEUE 4
#define NCLERKS 2
#define MAX_BUFFER 50
#define MICROSECOND_CONVERSION 100000
#define SHORTEST 1
#define LONGEST 0
#define DEFAULT_CU_ID -1
#define VALID_CU 0
#define INVALID_CU -1

//String Constants
#define READ "r"
#define CU_ARRIVES "Customer arrives with ID:"
#define CU_ENTERS_PRE "Customer enters queue:"
#define CU_ENTERS_SUFF ", and length"
#define CL "A clerk"
#define CL_SERVE "serving a customer:"
#define CU_ID ", the customer ID"
#define CL_ID ", the clerk ID"
#define START "starts"
#define FINISHES "finishes"
#define START_TIME "start time"
#define END_TIME "end time"
#define AT_TIME "at time"
#define AVG_WAIT_TIME_PRE "The average waiting time for all customers in the system is:"
#define AVG_WAIT_TIME_SUFF "seconds."
#define DELIMITERS ":,"
#define ERROR_INPUT_FILE "ERROR: incorrect num args or cannot open input file"

typedef struct {
  int customer_id;
  int service_time;
  int arrival_time;
} customer_info;

typedef struct {
  int clerk_num;
} clerk_info;

typedef struct node {
    int customer_id;
    struct node *next;
} node_t;

pthread_mutex_t queue_lengths_lock;
pthread_mutex_t q_lock;
pthread_mutex_t clerk_locks[NCLERKS];
pthread_mutex_t empty_queues_lock;
pthread_mutex_t clerk_serving_lock;
pthread_mutex_t total_wait_time_lock;
pthread_mutex_t init_time_lock;
pthread_cond_t q_conds[NQUEUE];
pthread_cond_t clerk_conds[NCLERKS];
pthread_cond_t empty_queues_cond;
int queue_lengths[NQUEUE];
node_t * queues [NQUEUE];
int serve_customer_ids[NCLERKS];
struct timeval init_time;
double overall_waiting_time;

void initLocks();
void initConVars();
int retrieveQueueNumber(int flag);
int isBeingServed(int cl_index);
double getCurrentSimulationTime(struct timeval * start_time); 
void updateTotalWaitTime(struct timeval * queue_time);
void enqueue(node_t** head, int val);
int dequeue(node_t** head);
int peek(node_t** head);
void print_list(node_t* head);
