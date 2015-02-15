#include "Jobs.h"

void *jobThreadMethod(void *input){
  Job *jobData = (Job*)input;
  printf("Running Job Thread. Security: %d\n", jobData->level);

  return 0;
}

int initJobStruct(Job *job, SecurityLevel level){
  job->level = level;
  job->nextJob = NULL;
  if (pthread_mutex_init(&(job->threadLock), NULL) != 0){
    printf("\nMutex init failed\n");
    return 1;
  }
  if(pthread_create(&(job->jobThread), NULL, jobThreadMethod, job)){
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }
  return 0;
}

void initTheseJobs(SecurityLevel level){
  int count;
  switch(level){
    case UNCLASSIFIED:
    count = UNCLASSIFIED_THREAD_COUNT;
    break;
    case SECRET:
    count = SECRET_THREAD_COUNT;
    break;
    case TOP_SECRET:
    count = TOP_SECRET_THREAD_COUNT;
    break;
    default:
    errorWithContext("Invalid level selection\n");
    break;
  }
  for(int i = 0; i<count; i++){
    Job *job = malloc(sizeof(Job));
    initJobStruct(job, level);
  }
}
