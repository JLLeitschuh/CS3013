#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "Error.h"
#include "Jobs.h"
#include "SecurityLevel.h"
#include "ClusterManager.h"


void testMode(){
  printf("Done with inintialization\n");

  usleep(5000000);
  Job *unsecured;
  if(removeFirstJobType(&unsecured, UNCLASSIFIED)){
    errorWithContext("Could not retrive element\n");
  } else{
    printf("Sucsess! Got job %d\n", unsecured->jobNumber);
  }
  printJobListStats();
  Job *topSecured;
  if(removeFirstJobType(&topSecured, TOP_SECRET)){
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
  initClusterStuff();
  printJobListStats();

  /*
   * All implied thread saftey ends here.
   *
   * There's no turning back now
   */
  initTheseJobs(UNCLASSIFIED);
  initTheseJobs(SECRET);
  initTheseJobs(TOP_SECRET);

  if(argc > 1){
    testMode();
  }

  manageCluster();

}
