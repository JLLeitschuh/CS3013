#ifndef _JOBS_H_
#define _JOBS_H_

#include "SecurityLevel.h"
#include "Error.h"
#include "Random.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define UNCLASSIFIED_THREAD_COUNT 8
#define SECRET_THREAD_COUNT 6
#define TOP_SECRET_THREAD_COUNT 6

typedef struct Job {
  unsigned int jobNumber;
  SecurityLevel level;
  pthread_mutex_t threadLock;
  pthread_t jobThread;
  int isTail;
  struct Job *nextJob;
} Job;

int initJobStuff();

int removeFirstJob(Job **returnJob, SecurityLevel level);

void addJobToList(Job *job);

void getJobListStats(int *us_count, int *s_count, int *ts_count);

void *jobThreadMethod(void *input);

int initJobStruct(Job *job, SecurityLevel level);

void initTheseJobs(SecurityLevel level);

void printJobListStats();


#endif //_JOBS_H_
