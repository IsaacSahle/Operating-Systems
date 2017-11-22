#include "diskinfo.h"

int main(int argc,char * argv[]){
  FILE * fp;
  if(argc != 2 || (fp = fopen(argv[1],"rb")) == NULL){
    perror("ERROR");
    return EXIT_FAILURE;
  }
  char os_name[OS_NAME_BYTES + 1];
  char disk_label[DISK_LABEL_BYTES + 1];
  char bps[BPS];
  char sector_count[SC];
  char sector_per_FAT[SF];
  char num_FAT[NF];
  char temp[DISK_LABEL_BYTES + 1];
  // OS name
  retrieve_bytes(fp,OS_NAME,OS_NAME_BYTES,os_name);
  os_name[OS_NAME_BYTES] = '\0';

  // Bytes per sector
  retrieve_bytes(fp,BPS_START,BPS,bps); 
  
  // Sector count
  retrieve_bytes(fp,SC_START,SC,sector_count);
  
  //Sector per FAT
  retrieve_bytes(fp,SF_START,SF,sector_per_FAT);
 
  // Number of FATs
  retrieve_bytes(fp,FATS_N,NF,num_FAT);
  
  int bytes_per_sc = buffer_to_int(bps);
  int sc_per_FAT = buffer_to_int(sector_per_FAT);
  int sc_count = buffer_to_int(sector_count);
  int number_of_FAT = (int) num_FAT[0];
  
  // Free Size of the Disk
  int FAT1_start = (1) * bytes_per_sc;
  int FAT1_size = (sc_per_FAT * bytes_per_sc);
  char FAT1_buffer[FAT1_size + 1];
  retrieve_bytes(fp,FAT1_start,FAT1_size,FAT1_buffer);
  FAT1_buffer[FAT1_size] = '\0';
  int free_size = free_disk_size(sector_count,FAT1_buffer);
  // printf("%s\n",FAT1_buffer);
  
  
  
  // Disk Label
  int root_start = (1 + (number_of_FAT * sc_per_FAT)) * bytes_per_sc;
  char dir_entry[32 + 1];
  retrieve_disk_label(fp,root_start,dir_entry);
  dir_entry[32] = '\0';
  
  //Files in Root dir
  int files_root_dir = 
  
  
  
  
  
  printf("%s\n",os_name);
  printf("%d\n",bytes_per_sc);
  printf("%d\n",number_of_FAT);
  printf("%d\n",sc_count);
  printf("%d\n",sc_per_FAT);
  printf("%s\n",dir_entry);
  //printf("%s\n",temp);

  fclose(fp);
  // OS name bytes 3-11
  return 0;
}
