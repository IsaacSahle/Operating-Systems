#include "diskinfo.h"

int buffer_to_int(char * buffer){
  return ((int) buffer[0] | (int) buffer[1] << 8);
}

void retrieve_bytes(FILE * fp,int start,int num_bytes,char * buffer){
  //Seek point in file
  fseek(fp,start,SEEK_SET);
  fread(buffer,num_bytes,1,fp); 
}

void retrieve_disk_label(FILE * fp,int root_start,char * buffer){ 
   while(1){
	// Read a dir entry which is 32 bytes
	retrieve_bytes(fp,root_start,32,buffer);
	if(buffer[11] != 0x0f && (buffer[11] & 0x08) == 0x08){
		break;
	}
	root_start += 32;
   }
}
