#include <stdio.h>
#include <stdlib.h>

//INTEGER CONSTANTS
#define DISK_INFO 55
#define OS_NAME_BYTES 8 
#define DISK_LABEL_BYTES 11
#define OS_NAME_START 3
#define OS_NAME 3
#define OS_NAME_FINISH 10
#define BPS_START 11
#define BPS_FINISH 12
#define SC_START 19
#define SC_FINISH 20
#define SF_START 22
#define SF_FINISH 23
#define DISK_LABEL_ATTR 0x08
#define BPS 2
#define SC 2
#define SF 2
#define NF 1
#define FATS_N 16

//STRING CONSTANTS
#define ERROR_NUM_ARGS "ERROR: incorrect number of arguments"
#define READ "r"

int buffer_to_int(char * buffer);
void retrieve_bytes(FILE * fp,int start,int num_bytes,char * buffer);
void retrieve_disk_label(FILE * fp,int root_start,char * buffer);
