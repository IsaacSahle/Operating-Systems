#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Integer Constants
#define NQUEUE 4
#define NCLERKS 2
#define MAX_BUFFER 10
#define MICROSECOND_CONVERSION 100000
#define SHORTEST 1
#define LONGEST 0

//String Constants
#define READ "r"
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
pthread_mutex_t q_locks[NQUEUE];
pthread_mutex_t clerk_locks[NCLERKS];
pthread_mutex_t empty_queues_lock;
pthread_mutex_t clerk_serving_lock;
pthread_cond_t q_conds[NQUEUE];
pthread_cond_t clerk_conds[NCLERKS];
pthread_cond_t empty_queues_cond;
int queue_lengths[NQUEUE];
node_t * queues [NQUEUE];
int serve_customer_ids[NCLERKS];

void initLocks();
void initConVars();
int retrieveQueueNumber(int flag);
int retrieveClerk(int c_id);
int isValidNextCustomer(int queue_num,int c_id);
void enqueue(node_t** head, int val);
int dequeue(node_t** head);
int peek(node_t** head);
void print_list(node_t* head);
