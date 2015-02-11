#include <pthread.h>
#include <stdio.h>
#include "secOS.h"

void *secretThread(void *input){
  printf("Running secret thread\n");
  SecurityLevel level;

 // while(1){
    //enterCluster(level);
    //We are now running in the cluster so do the job
    //Delay here for random time between .25 and 2 seconds
    //Leave cluster
    //exitCluster(level);
    //Another delay for some random ammount of time
//  }

}


int main(){
  const int threadCount = 20;
  pthread_t secretThreads[threadCount];
  int i;
  for(i = 0; i < threadCount; i++){
	  if(pthread_create(&(secretThreads[i]), NULL, secretThread, NULL)) {
	    fprintf(stderr, "Error creating thread\n");
	    return 1;
	  }
  }
  printf("Running main thread\n");

  //int i;
  for(i = 0; i < threadCount; i++){
	  if(pthread_join(secretThreads[i], NULL)) {
	    fprintf(stderr, "Error joining thread\n");
	    return 2;
	  }
  }

  //create all 20 threads: 8 unclassified, 6 secret, and 6 top secret 
/*	job job1;
	job job2;
	job job3;
	job job4;
	job job5;
	job job6;
	job job7;
	job job8;
	job job9;
	job job10;
	job job11;
	job job12;
	job job13;
	job job14;
	job job15;
	job job16;
	job job17;
	job job18;
	job job19;
	job job20;

	job1.SecurityLevel = TOP_SECRET
	job2.SecurityLevel = TOP_SECRET
	job3.SecurityLevel = TOP_SECRET */


  return 0;
}

