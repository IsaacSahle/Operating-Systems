#include "diskinfo.h"

int main(int argc, char * argv[]){
  FILE * fp;
  if(argc != 2 || (fp = fopen(argv[1],"rb")) == NULL){
    perror("ERROR");
    return EXIT_FAILURE;
  }
  
  unsigned char bps[BPS];
  unsigned char sector_per_FAT[SF];
  unsigned char num_FAT[NF];
  
  retrieve_bytes(fp,BPS_START,BPS,bps); 
  retrieve_bytes(fp,SF_START,SF,sector_per_FAT);
  retrieve_bytes(fp,FATS_N,NF,num_FAT);

  int bytes_per_sc = buffer_to_int(bps);
  int sc_per_FAT = buffer_to_int(sector_per_FAT);
  int number_of_FAT = (int) num_FAT[0];
 
  int root_start = (1 + (number_of_FAT * sc_per_FAT)) * bytes_per_sc;
  
  root_dir_list(fp,root_start);
  fclose(fp);

  return 0;
}
