#include "diskinfo.h"
int main(int argc,char * argv[]){
  FILE * fp;
  if(argc != 3 || (fp = fopen(argv[1],"rb")) == NULL){
    fprintf(stderr,"ERROR cannot open file or incorrect num of args");
    return EXIT_FAILURE;
  }
  
  char arg_cpy[strlen(argv[2]) + 1];
  strcpy(arg_cpy,argv[2]);

  int index = 0;
  while((argv[2])[index]){
    (argv[2])[index] = toupper((argv[2])[index]);
    index++;
  }

  int f_size = file_size(fp,argv[2]);
  if(f_size <= 0){
    fprintf(stderr,"File does not exist on disk or is empty\n");
    return EXIT_FAILURE; 
  }

  int f_logic_sector = first_sector(fp,argv[2]);
  int fat_entry_num = f_logic_sector;
  int byte_count = f_size;
  int address;
  int i = 0;
  unsigned char byte_buffer[1];  
  FILE * local_copy = fopen(arg_cpy,"w");
  if(local_copy == NULL){
    fprintf(stderr,"Cannot creat file %s\n",argv[2]);
    return EXIT_FAILURE;
  } 

   while(1){
    if(byte_count == 0){
        break;
     }

    if(f_size == byte_count){
      fat_entry_num = f_logic_sector; 
    }else{
      fat_entry_num = retrieve_FAT_entry(fp,fat_entry_num,512);
    }
   

    address = (31 + fat_entry_num) * 512;
    while(i < 512){
      if(byte_count == 0){
        break;
      }
   
      retrieve_bytes(fp,(i+address),1,byte_buffer);
      fwrite(byte_buffer,1,sizeof(byte_buffer),local_copy);
      i++;
      byte_count--;
    }
    i = 0;
  
    }

   fclose(fp);
   fclose(local_copy);
  return 0; 
}
