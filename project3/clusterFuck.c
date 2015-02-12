#include <pthread.h>
#include <stdio.h>

pthread_mutex_t TS_CLUSTER_ENTRY_lock;
pthread_mutex_t TS_num_waiting_lock;
unsigned int TS_num_waiting_count =0;
pthread_mutex_t S_CLUSTER_ENTRY_lock;
pthread_mutex_t S_num_waiting_lock;
unsigned int S_num_waiting_count =0;
pthread_mutex_t U_CLUSTER_ENTRY_lock;
pthread_mutex_t U_num_waiting_lock;
unsigned int U_num_waiting_count =0;



void _setError(const char *contextMessage, const char *filename, const char *function, int lineNumber);

#define errorWithContext(context)   _setError((context), __FILE__, __FUNCTION__, __LINE__)
#define errorNoContext()   errorWithContext("An error occurred")

void _setError(const char *contextMessage, const char *filename, const char *function, int lineNumber){
  printf("ERROR: %s File: %s Function: %s Line: %i\n\r", contextMessage, filename, function, lineNumber);
}


typedef enum {
  TOP_SECRET = 0,
  SECRET = 1,
  UNCLASSIFIED = 2,
  NONE = -1
} SecurityLevel;

SecurityLevel cluster[2];


//This handles having a backlog of jobs trying to enter the cluster
void enterCluster(SecurityLevel level, unsigned int *clusterNumber) {
  switch(level){
    case TOP_SECRET:
      pthread_mutex_lock(&TS_num_waiting_lock);
      TS_num_waiting_count++;
      pthread_mutex_unlock(&TS_num_waiting_lock);

      pthread_mutex_lock(&TS_CLUSTER_ENTRY_lock); //Wait for main to let me go

      pthread_mutex_lock(&TS_num_waiting_lock);
      TS_num_waiting_count--;
      pthread_mutex_unlock(&TS_num_waiting_lock);
      break;
    case SECRET:
      pthread_mutex_lock(&S_num_waiting_lock);
      S_num_waiting_count++;
      pthread_mutex_unlock(&S_num_waiting_lock);

      pthread_mutex_lock(&S_CLUSTER_ENTRY_lock); //Wait for main to let me go

      pthread_mutex_lock(&S_num_waiting_lock);
      S_num_waiting_count--;
      pthread_mutex_unlock(&S_num_waiting_lock);
      break;
    case UNCLASSIFIED:
      pthread_mutex_lock(&U_num_waiting_lock);
      U_num_waiting_count++;
      pthread_mutex_unlock(&U_num_waiting_lock);

      pthread_mutex_lock(&U_CLUSTER_ENTRY_lock); //Wait for main to let me go

      pthread_mutex_lock(&U_num_waiting_lock);
      U_num_waiting_count--;
      pthread_mutex_unlock(&U_num_waiting_lock);
      break;
    default:
      printf("ERROR: Undefined security level %d\n", level);
      break;
  }

  //We were origignally thinking about having the threads manage putting thesleves into the cluster but decided
  //that it was better if this was managed by the main thred.

  // int i;
  // for(i = 0; i < 3; i++){ //Iterate over the two .5 clusters
  //   if(i == 2){ //If I hit three then I'm not in a cluster
  //     errorWithContext("No space in cluster but I was allowed to enter it by main... Fail");
  //     exit(1);
  //
  //   }
  //   //There should be more error checking logic here like making sure incompatible
  //   //Threads aren't trying to enter the cluster
  //
  //
  //   if(cluster[i] == NONE){ //When we find the empty cluster
  //     clusterNumber* = i;
  //     cluster[i] = level;
  //     break;
  //   }
  // }
  // //Unlock the main thread that manages these things
}

void exitCluster(SecurityLevel level){
  //TODO: Write me
  //I need to tell main that I'm done
  //And then return once main acknowledges that
}


void *jobThread(void *input){
  SecurityLevel *levelPtr = (SecurityLevel*)(input);
  SecurityLevel level = *levelPtr;
  printf("Running secret thread: %d\n", level);
  while(1){
    unsigned int clusterNumber;
    enterCluster(level, &clusterNumber);
    printf("I'm running now %d\n", level);
    //We are now running in the cluster so do the job
    //Delay here for random time between .25 and 2 seconds
    //Leave cluster
    exitCluster(level);
    //Another delay for some random ammount of time
  }
}

int isClusterUnsecure(){
  if(cluster[0] == UNCLASSIFIED || cluster[1] == UNCLASSIFIED){
    return 1;
  }
  return 0;
}

void runThreadManager(){
  while(1){
    //Here we go round-about and unlock random threads.
    if(cluster[0] != NONE && cluster[1] != NONE) continue;
    //At this point at least one .5 cluster is empty
    if(TS_num_waiting_count >=3){
      //We have to get a TS one in but the cluster is unsecure
      if(isClusterUnsecure()) continue;
      //We now know that the cluster is secure
      int i;
      for(i = 0; i < 2; i++){
        if(cluster[i] = NONE){
          //We've found an empty spot for this cluster
          //Pop its mutex so that it begins to run
          pthread_mutex_unlock(&TS_CLUSTER_ENTRY_lock);
          cluster[i] = TOP_SECRET;
          continue;
        }
      }
    } else { //We dont have three TS jobs waiting so we can do this randomly
      //Check to see what job is in the cluster currently
      //If top secret or secret then see if there is a counterpart to match it
      //If not then loop
    }
  }
}

SecurityLevel getSecurityLevel(int number){
  if( number < 8){
    return UNCLASSIFIED;
  } else if( number < 14){
    return SECRET;
  } else if(number < 21){
    return TOP_SECRET;
  } else {
    errorWithContext("OUT OF RANGE YOU SCREED UP\n");
  }
}


int main(){
  const int threadCount = 20;

  if (pthread_mutex_init(&TS_CLUSTER_ENTRY_lock, NULL) != 0){
    printf("\nTS mutex init failed\n");
    return 1;
  }

  if (pthread_mutex_init(&S_CLUSTER_ENTRY_lock, NULL) != 0){
    printf("\nS mutex init failed\n");
    return 1;
  }

  if (pthread_mutex_init(&U_CLUSTER_ENTRY_lock, NULL) != 0){
    printf("\nU mutex init failed\n");
    return 1;
  }

  if (pthread_mutex_init(&TS_num_waiting_lock, NULL) != 0){
    printf("\nTS num waiting mutex init failed\n");
    return 1;
  }

  if (pthread_mutex_init(&S_num_waiting_lock, NULL) != 0){
    printf("\nS num waiting mutex init failed\n");
    return 1;
  }

  if (pthread_mutex_init(&U_num_waiting_lock, NULL) != 0){
    printf("\nU num waiting mutex init failed\n");
    return 1;
  }

  //TODO initialize the other mutexes here

  printf("Beging Job generation\n");

  pthread_t jobThreads[threadCount];
  int i;
  for(i = 0; i < threadCount; i++){
    printf("Creating Job %d\n", i);
    SecurityLevel *level;
    *level = getSecurityLevel(i);
	  if(pthread_create(&(jobThreads[i]), NULL, jobThread, level)) {
	    fprintf(stderr, "Error creating thread\n");
	    return 1;
	  }
  }
  printf("Running main thread\n");


  runThreadManager();

  return 0;
}
