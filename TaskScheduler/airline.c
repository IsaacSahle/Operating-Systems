#include "airline.h"

struct timeval init_time;
double overall_waiting_time;
int queue_length[NQUEUE];
 
int main() {
  // initialize all the condition variable and thread lock will be used
  initLocks();
  initConVars(); 
  return 0;
}
