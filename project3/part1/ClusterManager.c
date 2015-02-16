#include "ClusterManager.h"

pthread_mutex_t clusterMutex;
SecurityLevel cluster[2];

//Initialize all mutexex and other nessasary variables
int initClusterStuff(){
  if (pthread_mutex_init(&clusterMutex, NULL) != 0){
    printf("\nclusterMutex mutex init failed\n");
    return 1;
  }
  cluster[0] = NONE;
  cluster[1] = NONE;
  return 0;
}

Bool isThreeTSWaiting(){
  int us_count=0;
  int s_count =0;
  int ts_count=0;
  getJobListCountStats(&us_count, &s_count, &ts_count);
  return ts_count > 2;
}

Bool isClusterUnsecured(){
  Bool unsecured = false;
  pthread_mutex_lock(&clusterMutex);
  if(cluster[0] == UNCLASSIFIED || cluster[1] == UNCLASSIFIED){
    unsecured = true;
  }
  pthread_mutex_unlock(&clusterMutex);
  return unsecured;
}

Bool isClusterAvailable(){
  Bool available = false;
  //Lock the cluster while we check that it is available
  pthread_mutex_lock(&clusterMutex);
  //If either cluster is free then the cluster is free
  if(cluster[0] == NONE  || cluster[1] == NONE){
    available = true;
  }
  pthread_mutex_unlock(&clusterMutex);
  return available;
}

Bool isClusterEmpty(){
  Bool empty = false;
  //Lock the cluster while we check that it is available
  pthread_mutex_lock(&clusterMutex);
  //If both cluster halves are free then the cluster is free
  if(cluster[0] == NONE  && cluster[1] == NONE){
    empty = true;
  }
  pthread_mutex_unlock(&clusterMutex);
  return empty;
}


//This should only ever be run by the cluster manager
void enterCluster(Job *job){
  if(job->level == NONE){
    errorWithContext("You tried to enter the cluster with a job level of NONE\n");
    exit(1);
  }
  int clusterUsed = -1;
  pthread_mutex_lock(&clusterMutex);
  if(cluster[0] == NONE){
    cluster[0] = job->level;
    job->inCluster = 0;
  } else if (cluster[1] == NONE){
    cluster[1] = job->level;
    job->inCluster = 1;
  } else {
    errorWithContext("[Mistakes were made!] Tried to enter the cluster but no spaces were available\n");
    exit(1);
  }
  pthread_mutex_unlock(&clusterMutex);
  //Allow the job to enter the cluster
  pthread_mutex_unlock(&(job->threadLock));
  printf("[ENTERING CLUSTER] Job Num: %d, Security Level: %d, Cluster Num: %d\n", job->jobNumber,job->level, job->inCluster );
}

void exitCluster(Job *job){
  printf("[EXITING CLUSTER] Job Num: %d, Security Level: %d, Cluster Num: %d\n", job->jobNumber,job->level, job->inCluster );
  pthread_mutex_lock(&clusterMutex);
  if(cluster[job->inCluster] != job->level){
    errorWithContext("Someone changed the cluster level while another job was in that half\n");
    exit(1);
  }
  cluster[job->inCluster] = NONE;
  job->inCluster = -1;
  pthread_mutex_unlock(&clusterMutex);

}


/*
 * This is the master method that is used to manage all of the cluster
 */
void manageCluster(){
  while(true){
    //printf("Manage cluster loop\n");
    //If the cluster is not available then continue to loop
    if(!isClusterAvailable()) continue;

    //If there are three TS waiting
    if(isThreeTSWaiting()){
      Job *ts_job = NULL;
      if(removeFirstJobType(&ts_job, TOP_SECRET)){
        errorWithContext("There was no TS job to retrive\n");
        exit(0);
      }
      enterCluster(ts_job);

      //Wait for the cluster to be available
      while(!isClusterAvailable());
      ts_job = NULL;

      if(removeFirstJobType(&ts_job, TOP_SECRET)){
        errorWithContext("There was no TS job to retrive");
        exit(0);
      }
      enterCluster(ts_job);
      continue;
    }

    //If there is not a job in the queue
    if(!isJobInQueue()) continue;

    if(isClusterEmpty()){
      //We should pick a job to run
      Job *nextJob = NULL;
      if(removeFirstJob(&nextJob)){
        errorWithContext("Failed to get the next job\n");
      }
      enterCluster(nextJob);
      continue;
    } else { //If the cluster is not empty
      if(isClusterUnsecured()){
        Job *unclassifiedJob = NULL;
        if(removeFirstJobType(&unclassifiedJob, UNCLASSIFIED)){
          //printf("No unsecured jobs avaiable\n");
          continue; //Nothing can be done from here
        }
        enterCluster(unclassifiedJob);
        continue;
      } else { //The cluster is secure
        Job *securedJob = NULL;
        if(removeSimilarJob(&securedJob, SECRET)){
          //printf("No secured jobs avaiable\n");
          continue; //Nothing can be done from here
        }
        enterCluster(securedJob);
        continue;
      }
    }
  }
}
