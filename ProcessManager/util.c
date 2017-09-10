#include "rsi.h"

void insertNode(Node ** head ,pid_t pid){
 Node * insertionNode = malloc(sizeof(Node));
 insertionNode->processId = pid;
 insertionNode->next = *head;
 *head = insertionNode;
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
