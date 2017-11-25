#include "diskinfo.h"

int main(int argc,char * argv[]){
  FILE * fp;
  if(argc != 2 || (fp = fopen(argv[1],"rb")) == NULL){
    perror("ERROR");
    return EXIT_FAILURE;
  }
  unsigned char os_name[OS_NAME_BYTES + 1];
  unsigned char bps[BPS];
  unsigned char sector_count[SC];
  unsigned char sector_per_FAT[SF];
  unsigned char num_FAT[NF];
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
  int free_size = free_disk_size(fp,sc_count,bytes_per_sc); 
  
  // Disk Label
  int root_start = (1 + (number_of_FAT * sc_per_FAT)) * bytes_per_sc;
  unsigned char dir_entry[DIR_ENTRY + 1];
  retrieve_disk_label(fp,root_start,dir_entry);
  dir_entry[ATTR_BYTE] = '\0';
  
  //Files in Root dir
  int files_root_dir = root_dir_files(fp,root_start);
  
  printf("OS Name: %s\n",os_name);
  printf("Label of the disk: %s\n",dir_entry);
  printf("Total size of the disk: %d\n",(sc_count * bytes_per_sc));
  printf("Free size of the disk: %d\n",free_size);
  printf("Number of files in root dir (not including sub dirs): %d\n",files_root_dir);
  printf("Number of FAT copies: %d\n",number_of_FAT);
  printf("Sectors per FAT: %d\n",sc_per_FAT);

  fclose(fp);
  return 0;
}
