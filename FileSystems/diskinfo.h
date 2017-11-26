#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
#define IGNORE_ATTR 0x0f
#define SUB_DIR_ATTR 0x10
#define FIRST_BYTE 1
#define FREE 0xE5
#define EMPTY 0x00
#define DIR_ENTRY 32
#define ATTR_BYTE 11
#define MAX_ROOT_DIR 2
#define MAX_ROOT_DIR_BYTE 17
#define BPS 2
#define SC 2
#define SF 2
#define NF 1
#define FATS_N 16

//STRING CONSTANTS
#define ERROR_NUM_ARGS "ERROR: incorrect number of arguments"
#define READ "r"

int buffer_to_int(unsigned char * buffer);
void retrieve_bytes(FILE * fp,int start,int num_bytes,unsigned char * buffer);
void retrieve_disk_label(FILE * fp,int root_start,unsigned char * buffer);
unsigned char retrieve_FAT_entry(FILE * fp,int entry,int sector_count);
int root_dir_files(FILE * fp,int root_start);
void root_dir_list(FILE * fp,int root_start);
int free_disk_size(FILE * fp,int sector_count,int bps);
int root_dir_entries(FILE * fp,int root_start);
void file_info(unsigned char * dir_entry);
int file_size(FILE * fp,char * search_name);
int first_sector(FILE * fp,char * search_name);
