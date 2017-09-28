#include "rsi.h"

void insertNode(Node ** head ,pid_t pid){
 Node * insertionNode = malloc(sizeof(Node));
 insertionNode->processId = pid;
 insertionNode->next = *head;
 *head = insertionNode;
 sortList(head);
}

void removeNode(Node ** head ,pid_t pid){
if(*head == NULL){
  return;
}else if((*head)->processId == pid){
  Node * temp = (*head)->next;
  free(*head);
  *head = temp;
}else{
  Node * curr = *head;

  while(curr->next != NULL){
    if(curr->next->processId == pid){
      //found node
      break;
    }
   curr = curr->next;
  }
  
  if(curr->next == NULL){
    return;
  }else{
    Node * temp = curr->next->next;
    free(curr->next);
    curr->next = temp;
  }
}

}

int size(Node * head){
  int count = 0;
  Node * curr = head;
  while(curr != NULL){
    curr = curr->next;
    count++;
  }
  return count;
}

void sortList(Node ** head){
 //swap new item into list based on ascending pid
 Node * curr = *head;
 while(curr->next != NULL){
   if(curr->processId > curr->next->processId){
     pid_t temp = curr->next->processId;
     curr->next->processId = curr->processId;
     curr->processId = temp;
   }else{
     break;
   }
   curr = curr->next;
 }
}

int searchList(Node ** head, pid_t pid){
  Node * curr = *head;
  while(curr != NULL){
    if(curr->processId == pid){
      return 1;
    }
    curr = curr->next;
  }
  return 0; 
}
