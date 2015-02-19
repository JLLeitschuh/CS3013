#include "Jobs.h"

pthread_mutex_t jobListMutex;

Job *jobListStart;
int initJobStuff() {
	if (pthread_mutex_init(&jobListMutex, NULL) != 0) {
		printf("\njobAdd mutex init failed\n");
		return 1;
	}
	jobListStart = malloc(sizeof(Job));
	jobListStart->isTail = 1;
	jobListStart->level = NONE;
	return 0;
}

//Warning! This is not thread safe. Make sure you are using a lock!
void _addJob(Job *toJob, Job *addJob) {
	if (toJob->isTail) {
		//printf("Adding Job\n");
		toJob->isTail = 0;
		addJob->isTail = 1;
		toJob->nextJob = addJob;
		return;
	} else {
		//printf("Recursing\n");
		_addJob(toJob->nextJob, addJob);
		return;
	}
}

void addJobToList(Job *job) {
	//Prevent multple adds occuring at the same time
	pthread_mutex_lock(&jobListMutex);
	_addJob(jobListStart, job);
	pthread_mutex_unlock(&jobListMutex);
}

int removeFirstJob(Job ** returnJob) {
	int returnValue = 0;
	if (jobListStart->isTail)
		return 1;
	pthread_mutex_lock(&jobListMutex);
	*returnJob = jobListStart->nextJob;
	if (jobListStart->nextJob->isTail) {
		jobListStart->nextJob = NULL;
		jobListStart->isTail = true;
	} else {
		jobListStart->nextJob = (*returnJob)->nextJob;
		(*returnJob)->nextJob = NULL;
	}
	pthread_mutex_unlock(&jobListMutex);
	return returnValue;
}

/*
 * Removes a job that can run in the cluster with the matching job.
 * If TS or S will find the one with more jobs waiting and return that type
 * If it is a tie then will retrive the level matching
 */
int removeSimilarJob(Job **returnJob, SecurityLevel level) {
	if (level == NONE) {
		errorWithContext("Invalid level");
		return 1; //Invalid usage
	}
	int us_count = 0;
	int s_count = 0;
	int ts_count = 0;
	//Get the stats for comparison
	getJobListCountStats(&us_count, &s_count, &ts_count);

	if (level == UNCLASSIFIED) { //Why are you using this method then? But whatever...
		if (us_count > 0) {
			return removeFirstJobType(returnJob, level);
		} else {
			return 1;
		}
	} else { // Requesting a secured job
		if (s_count > ts_count) {
			return removeFirstJobType(returnJob, SECRET);
		} else if (s_count < ts_count) {
			return removeFirstJobType(returnJob, TOP_SECRET);
		} else { // They are equal
			if (ts_count == 0)
				return 1; // There is no job to retive
			return removeFirstJobType(returnJob, level);
		}
	}
}

/*
 * Returns true if there is a job in the queue
 */
Bool isJobInQueue() {
	int u_count = 0, s_count = 0, ts_count = 0;
	getJobListCountStats(&u_count, &s_count, &ts_count);
	return (u_count > 0 || s_count > 0 || ts_count > 0);
}

int _removeFirstJobType(Job **returnJob, Job *element, SecurityLevel level) {
	//printf("Iterating\n");
	//If this element is the tail then we have failed
	if (element->isTail) {
		//errorWithContext("Reached end of list\n");
		return 1;
	}

	//If the next element has the level we want to return
	if (element->nextJob->level == level) {
		*returnJob = element->nextJob;
		//If the next job is the last element of the list
		if ((*returnJob)->isTail) {
			element->isTail = 1;
		} else { //If the next job is not the tail
			element->nextJob = (*returnJob)->nextJob;
			(*returnJob)->nextJob = NULL;
			(*returnJob)->isTail = 0;
		}
	} else { //The next element is not the one we want
		return _removeFirstJobType(returnJob, element->nextJob, level);
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
int removeFirstJobType(Job **returnJob, SecurityLevel level) {
	//printf("Remove first job\n");
	if (level == NONE)
		return 1;
	int returnValue = 0;
	pthread_mutex_lock(&jobListMutex);
	returnValue = _removeFirstJobType(returnJob, jobListStart, level);
	pthread_mutex_unlock(&jobListMutex);
	return returnValue;
}

/*
 * Recusrive function to calculate the stats of the linked list
 * NOT THREAD SAFE. MUST USE LOCKS ARROUND IT!
 */
void _getJobListCountStats(Job *element, int *us_count, int *s_count,
		int *ts_count) {
	//Note: the paranthesees arround the pointer dereference are nessasary because otherwise you will be incrementing the pointer not the value
	switch (element->level) {
	case UNCLASSIFIED:
		(*us_count)++;
		break;
	case SECRET:
		(*s_count)++;
		break;
	case TOP_SECRET:
		(*ts_count)++;
		break;
	default:
		errorWithContext("Invalid Security Level\n");
		break;
	}
	//If this is a tail element then we need to stop recursing
	if (!element->isTail) {
		_getJobListCountStats(element->nextJob, us_count, s_count, ts_count);
	}
}

void getJobListCountStats(int *us_count, int *s_count, int *ts_count) {
	*us_count = 0;
	*s_count = 0;
	*ts_count = 0;
	//We need the lock because we don't want the list changing out from under us
	pthread_mutex_lock(&jobListMutex);
	if (!jobListStart->isTail) { //If the list only includes the jobListStart
		_getJobListCountStats(jobListStart->nextJob, us_count, s_count,
				ts_count);
	}
	pthread_mutex_unlock(&jobListMutex);
}

void printJobListStats() {
	//Resets all the values to 0 incase they are not initialized
	int us_count = 0;
	int s_count = 0;
	int ts_count = 0;
	//Get the stats
	getJobListCountStats(&us_count, &s_count, &ts_count);
	printf("[Count] US: %d, S: %d, TS: %d\n", us_count, s_count, ts_count);
}

void *jobThreadMethod(void *input) {
	Job *jobData = (Job*) input;
	//Down the mutex the first time so that the next time it is downed it will block
	pthread_mutex_lock(&(jobData->threadLock));
	long randomWaitTime = getRandomBetween(500000, 2000000);
	printf("Running Job %2d Thread. Security: %d Will delay for %7ld us\n",
			jobData->jobNumber, jobData->level, randomWaitTime);
	//Iterate infinitely
	while (true) {
		usleep(randomWaitTime);
		//Adds this job to the list of jobs waiting to run
		addJobToList(jobData);

		//Now we down our mutex twice so we are in the waiting state
		pthread_mutex_lock(&(jobData->threadLock));
		//If we are here we have entered the cluster! YEAY!
		long randomRunTime = getRandomBetween(500000, 2000000);
		usleep(randomRunTime);
		exitCluster(jobData);
    if(jobData->level == TOP_SECRET){
      randomWaitTime = getRandomBetween(15000000, 18000000);
    } else {
      randomWaitTime = getRandomBetween(4000000, 6000000);
    }
	}

	printf("Job %d is complete.\n", jobData->jobNumber);
	return 0;
}

int initCount = 0;
/*
 * Initialize the job struct with the given security level
 */
int initJobStruct(Job *job, SecurityLevel level) {
	job->jobNumber = initCount;
	job->level = level;
	//By default this element is a tail element
	job->isTail = 1;
	job->inCluster = -1;
	job->nextJob = NULL;

	initCount++;
	if (pthread_mutex_init(&(job->threadLock), NULL) != 0) {
		printf("\nMutex init failed\n");
		return 1;
	}
	if (pthread_create(&(job->jobThread), NULL, jobThreadMethod, job)) {
		fprintf(stderr, "Error creating thread\n");
		return 1;
	}
	return 0;
}

void initTheseJobs(SecurityLevel level) {
	int count;
	switch (level) {
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
	for (int i = 0; i < count; i++) {
		Job *job = malloc(sizeof(Job));
		initJobStruct(job, level);
	}
}


int getFirstSecurityLevel(SecurityLevel *level){
  if(jobListStart->isTail){
    return 1;
  }
  pthread_mutex_lock(&jobListMutex);
  *level = jobListStart->nextJob->level;
  pthread_mutex_unlock(&jobListMutex);
  return 0;
}

int getFirstJobNumber(int *number){
  if(jobListStart->isTail){
    return 1;
  }
  pthread_mutex_lock(&jobListMutex);
  *number = jobListStart->nextJob->jobNumber;
  pthread_mutex_unlock(&jobListMutex);
  return 0;
}
