#include "Jobs.h"

pthread_mutex_t jobListMutex;

Job *jobListStart;
int initJobStuff(){
  if (pthread_mutex_init(&jobListMutex, NULL) != 0){
    printf("\njobAdd mutex init failed\n");
    return 1;
  }
  jobListStart = malloc(sizeof(Job));
  jobListStart->isTail = 1;
  jobListStart->level = NONE;
  return 0;
}

//Warning! This is not thread safe. Make sure you are using a lock!
void _addJob(Job *toJob, Job *addJob){
  if(toJob->isTail){
    printf("Adding Job\n");
    toJob->isTail = 0;
    addJob->isTail = 1;
    toJob->nextJob = addJob;
    return;
  } else {
    printf("Recursing\n");
    _addJob(toJob->nextJob, addJob);
    return;
  }
}

void addJobToList(Job *job){
  //Prevent multple adds occuring at the same time
  pthread_mutex_lock(&jobListMutex);
  _addJob(jobListStart, job);
  pthread_mutex_unlock(&jobListMutex);
}

int _removeFirstJob(Job **returnJob, Job *element, SecurityLevel level){
  printf("Iterating\n");
  //If this element is the tail then we have failed
  if(element->isTail){
    errorWithContext("Reached end of list\n");
    return 1;
  }

  //If the next element has the level we want to return
  if( element->nextJob->level == level){
    printf("I'm here\n");
    *returnJob = element->nextJob;
    //If the next job is the last element of the list
    if((*returnJob)->isTail){
      element->isTail = 1;
    } else { //If the next job is not the tail
      element->nextJob = (*returnJob)->nextJob;
      (*returnJob)-> nextJob = NULL;
      (*returnJob)->isTail = 0;
    }
  } else { //The next element is not the one we want
    return _removeFirstJob(returnJob, element->nextJob, level);
  }
  //If we reach this time we sucseeded
  return 0;
}
/*
 * Removes the first job of a given type from the list
 * @param returnJob where to return the job to
 * @param level     the level to retrive
 * return  0 if sucsessful, 1 if failure
 *         failure can occur if the requested level job is not available
 */
int removeFirstJob(Job **returnJob, SecurityLevel level){
  if(level == NONE) return 1;
  int returnValue = 0;
  pthread_mutex_lock(&jobListMutex);
  returnValue = _removeFirstJob(returnJob, jobListStart, level);
  pthread_mutex_unlock(&jobListMutex);
  return returnValue;
}

/*
 * Recusrive function to calculate the stats of the linked list
 * NOT THREAD SAFE. MUST USE LOCKS ARROUND IT!
 */
void _getJobListStats(Job *element, int *us_count, int *s_count, int *ts_count){
  //Note: the paranthesees arround the pointer dereference are nessasary because otherwise you will be incrementing the pointer not the value
  switch(element->level){
    case UNCLASSIFIED:
      (*us_count) ++;
      break;
    case SECRET:
      (*s_count) ++;
      break;
    case TOP_SECRET:
      (*ts_count) ++;
      break;
    default:
      errorWithContext("Invalid Security Level\n");
      break;
  }
  //If this is a tail element then we need to stop recursing
  if(!element->isTail){
    _getJobListStats(element->nextJob, us_count, s_count, ts_count);
  }
}

void getJobListStats(int *us_count, int *s_count, int *ts_count){
  *us_count = 0;
  *s_count = 0;
  *ts_count = 0;
  //We need the lock because we don't want the list changing out from under us
  pthread_mutex_lock(&jobListMutex);
  if(!jobListStart->isTail){ //If the list only includes the jobListStart
    _getJobListStats(jobListStart->nextJob, us_count, s_count, ts_count);
  }
  pthread_mutex_unlock(&jobListMutex);
}

void printJobListStats(){
  //Resets all the values to 0 incase they are not initialized
  int us_count =0;
  int s_count  =0;
  int ts_count =0;
  //Get the stats
  getJobListStats(&us_count, &s_count, &ts_count);
  printf("[Count] US: %d, S: %d, TS: %d\n", us_count, s_count, ts_count);
}


void *jobThreadMethod(void *input){
  Job *jobData = (Job*)input;
  long randomWaitTime = getRandomBetween(500000, 2000000);
  printf("Running Job Thread. Security: %d Will delay for %ld us\n", jobData->level, randomWaitTime);
  usleep(randomWaitTime);
  addJobToList(jobData);
  

  return 0;
}

int initCount =0;
/*
 * Initialize the job struct with the given security level
 */
int initJobStruct(Job *job, SecurityLevel level){
  job->jobNumber = initCount;
  job->level = level;
  //By default this element is a tail element
  job->isTail = 1;
  job->nextJob =NULL;

  initCount++;
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
