#ifndef _JOBS_H_
#define _JOBS_H_

#include "SecurityLevel.h"
#include "Error.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define UNCLASSIFIED_THREAD_COUNT 8
#define SECRET_THREAD_COUNT 6
#define TOP_SECRET_THREAD_COUNT 6

struct Job;

typedef struct {
  SecurityLevel level;
  pthread_mutex_t threadLock;
  pthread_t jobThread;
  struct Job* nextJob;
} Job;

Job *jobListStart;

void *jobThreadMethod(void *input);

int initJobStruct(Job *job, SecurityLevel level);

void initTheseJobs(SecurityLevel level);


#endif //_JOBS_H_
