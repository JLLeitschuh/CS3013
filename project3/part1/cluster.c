#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "Error.h"
#include "Jobs.h"
#include "SecurityLevel.h"



Job *jobListStart;

int main(){
  initTheseJobs(UNCLASSIFIED);
  initTheseJobs(SECRET);
  initTheseJobs(TOP_SECRET);
  printf("Done with inintialization\n");
  while(1);
}
