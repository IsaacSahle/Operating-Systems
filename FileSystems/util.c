#include "diskinfo.h"

int buffer_to_int(unsigned char * buffer){
  return ((int) buffer[0] | (int) buffer[1] << 8);
}

void retrieve_bytes(FILE * fp,int start,int num_bytes,unsigned char * buffer){
  //Seek point in file
  fseek(fp,start,SEEK_SET);
  fread(buffer,num_bytes,1,fp); 
}

unsigned char retrieve_FAT_entry(FILE * fp,int entry,int bps){
  unsigned char result[1];
  unsigned char entry_bytes[2];
  int start = bps + ((3*entry)/2); 
	retrieve_bytes(fp,start,2,entry_bytes);

  if (entry % 2 == 0) {
    entry_bytes[1] = (entry_bytes[1] & 0x0F) << 8;
    entry_bytes[0] = entry_bytes[0] & 0xFF;
    result[0] = entry_bytes[0] | entry_bytes[1];
  } else {
   	entry_bytes[0] =(entry_bytes[0] & 0xF0);
    entry_bytes[1] = (entry_bytes[1] & 0xFF);
    result[0] = (entry_bytes[0] >> 4) + (entry_bytes[1] << 4);
  }

  return result[0];    
}

void retrieve_disk_label(FILE * fp,int root_start,unsigned char * buffer){ 
	int max_root_entries = root_dir_entries(fp,root_start);
	int i;
	for(i = 0; i < max_root_entries;i++){
		// Read a dir entry which is 32 bytes
		retrieve_bytes(fp,root_start,DIR_ENTRY,buffer);
		if(buffer[ATTR_BYTE] != IGNORE_ATTR && (buffer[ATTR_BYTE] & DISK_LABEL_ATTR) == DISK_LABEL_ATTR){
			break;
		}
		root_start += DIR_ENTRY;
	}
}

int root_dir_files(FILE * fp,int root_start){
	unsigned char dir_entry[DIR_ENTRY];
	int max_root_entries = root_dir_entries(fp,root_start);
	int i,count = 0;
	for(i = 0; i < max_root_entries;i++){
		retrieve_bytes(fp,root_start,DIR_ENTRY,dir_entry);
		if(dir_entry[ATTR_BYTE] != IGNORE_ATTR 
			&& (dir_entry[ATTR_BYTE] & DISK_LABEL_ATTR) != DISK_LABEL_ATTR
			&& dir_entry[FIRST_BYTE] != FREE
			&& dir_entry[FIRST_BYTE] != EMPTY
			&& (dir_entry[ATTR_BYTE] & SUB_DIR_ATTR) != SUB_DIR_ATTR){
			count++;
		}
		
		root_start += DIR_ENTRY;
	}
	return count;
}

void root_dir_list(FILE * fp,int root_start){
	unsigned char dir_entry[DIR_ENTRY];
	int max_root_entries = root_dir_entries(fp,root_start);
  char file_type;
  char file_name[13];
  int file_size;
	int i,count = 0;
	for(i = 0; i < max_root_entries;i++){
		retrieve_bytes(fp,root_start,DIR_ENTRY,dir_entry);
		if(dir_entry[ATTR_BYTE] != IGNORE_ATTR 
			&& (dir_entry[ATTR_BYTE] & DISK_LABEL_ATTR) != DISK_LABEL_ATTR
			&& dir_entry[FIRST_BYTE] != FREE
			&& dir_entry[FIRST_BYTE] != EMPTY){
        if((dir_entry[ATTR_BYTE] & SUB_DIR_ATTR) == SUB_DIR_ATTR){
          //sub dir
          file_type = 'D';
        }else{
          //file
          file_type = 'F';
        }
        
        int j;
        for(j = 0; j < 8;j++){
          if(dir_entry[j] == ' '){
            break;
          }
          file_name[j] = dir_entry[j]; 
        }

        file_name[j] = '.';
        j++;

       for(count = 0;count < 3;count++){
          file_name[j] = dir_entry[8 + count];
          j++;
       }

    
      file_name[j] = '\0'; 
      
      file_size = (dir_entry[28] & 0xFF) + ((dir_entry[29] & 0xFF) << 8) + ((dir_entry[30] & 0xFF) << 16) + ((dir_entry[31] & 0xFF) << 24);
 
      // Reference: tutorial resources in connex
      int time, date;
      int hours, minutes, day, month, year;
      time = *(unsigned short *)(dir_entry + 14);
      date = *(unsigned short *)(dir_entry + 16);
  
     //the year is stored as a value since 1980
     //the year is stored in the high seven bits
     year = ((date & 0xFE00) >> 9) + 1980;
     //the month is stored in the middle four bits
     month = (date & 0x1E0) >> 5;
     //the day is stored in the low five bits
     day = (date & 0x1F);
     //the hours are stored in the high five bits
     hours = (time & 0xF800) >> 11;
     //the minutes are stored in the middle 6 bits
     minutes = (time & 0x7E0) >> 5;
      printf("%c %10d %20s %d-%02d-%02d %02d:%02d\n",file_type,file_size,file_name,year,month,day,hours,minutes);
		}
		
		root_start += DIR_ENTRY;
	}
}
 
int root_dir_entries(FILE * fp,int root_start){
	unsigned char max_root_buffer[MAX_ROOT_DIR];
	retrieve_bytes(fp,MAX_ROOT_DIR_BYTE,MAX_ROOT_DIR,max_root_buffer);
	return buffer_to_int(max_root_buffer);
}

int file_size(FILE * fp, char * search_name){
	unsigned char dir_entry[DIR_ENTRY];
  char file_name[13];

  int root_start = (19) * 512;

	int max_root_entries = root_dir_entries(fp,root_start);
	int i;
	for(i = 0; i < max_root_entries;i++){
		retrieve_bytes(fp,root_start,DIR_ENTRY,dir_entry);
		if(dir_entry[ATTR_BYTE] != IGNORE_ATTR 
			&& (dir_entry[ATTR_BYTE] & DISK_LABEL_ATTR) != DISK_LABEL_ATTR
			&& dir_entry[FIRST_BYTE] != FREE
			&& dir_entry[FIRST_BYTE] != EMPTY
			&& (dir_entry[ATTR_BYTE] & SUB_DIR_ATTR) != SUB_DIR_ATTR){
		  	
        int j;
        for(j = 0; j < 8;j++){
          if(dir_entry[j] == ' '){
            break;
          }
          file_name[j] = dir_entry[j]; 
        }

        file_name[j] = '.';
        j++;

       int count;
       for(count = 0;count < 3;count++){
          file_name[j] = dir_entry[8 + count];
          j++;
       }
      
       file_name[j] = '\0';
       if(strcmp(file_name,search_name) == 0){
          return (dir_entry[28] & 0xFF) + ((dir_entry[29] & 0xFF) << 8) + ((dir_entry[30] & 0xFF) << 16) + ((dir_entry[31] & 0xFF) << 24);
       }
		}
		
		root_start += DIR_ENTRY;
	}
 return -1; 
}

int free_disk_size(FILE * fp,int sector_count,int bps){
   int i;
   int free_count = 0;
   for(i = 2; i < (sector_count - 31); i++){
	   if(retrieve_FAT_entry(fp,i,bps) == 0x00){
		   free_count++;
	   }
   }
	return (free_count * bps);
}

int first_sector(FILE * fp, char * search_name){
	unsigned char dir_entry[DIR_ENTRY];
  char file_name[13];

  int root_start = (19) * 512;

	int max_root_entries = root_dir_entries(fp,root_start);
	int i;
	for(i = 0; i < max_root_entries;i++){
		retrieve_bytes(fp,root_start,DIR_ENTRY,dir_entry);
		if(dir_entry[ATTR_BYTE] != IGNORE_ATTR 
			&& (dir_entry[ATTR_BYTE] & DISK_LABEL_ATTR) != DISK_LABEL_ATTR
			&& dir_entry[FIRST_BYTE] != FREE
			&& dir_entry[FIRST_BYTE] != EMPTY
			&& (dir_entry[ATTR_BYTE] & SUB_DIR_ATTR) != SUB_DIR_ATTR){
		  	
        int j;
        for(j = 0; j < 8;j++){
          if(dir_entry[j] == ' '){
            break;
          }
          file_name[j] = dir_entry[j]; 
        }

        file_name[j] = '.';
        j++;

       int count;
       for(count = 0;count < 3;count++){
          file_name[j] = dir_entry[8 + count];
          j++;
       }
      
       file_name[j] = '\0';
       if(strcmp(file_name,search_name) == 0){
          return (dir_entry[26]) + (dir_entry[27] << 8);
       }
		}
		
		root_start += DIR_ENTRY;
	}
 return -1; 
}
