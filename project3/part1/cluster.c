#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "Error.h"
#include "Jobs.h"
#include "SecurityLevel.h"


void testMode(){
  printf("Done with inintialization\n");

  usleep(5000000);
  Job *unsecured;
  if(removeFirstJob(&unsecured, UNCLASSIFIED)){
    errorWithContext("Could not retrive element\n");
  } else{
    printf("Sucsess! Got job %d\n", unsecured->jobNumber);
  }
  printJobListStats();
  Job *topSecured;
  if(removeFirstJob(&unsecured, TOP_SECRET)){
    errorWithContext("Could not retrive element\n");
  } else{
    printf("Sucsess! Got job %d\n", topSecured->jobNumber);
  }
  printJobListStats();
  usleep(1000000);
  while(1){
    printJobListStats();
  };
}


int main(int argc, char *argv[]){
  srandom(1);
  initJobStuff();
  printJobListStats();
  initTheseJobs(UNCLASSIFIED);
  initTheseJobs(SECRET);
  initTheseJobs(TOP_SECRET);



}
