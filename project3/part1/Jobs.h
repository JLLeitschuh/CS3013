#ifndef _JOBS_H_
#define _JOBS_H_

#include "SecurityLevel.h"
#include "Error.h"
#include "Random.h"
#include "Boolean.h"
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define UNCLASSIFIED_THREAD_COUNT 8
#define SECRET_THREAD_COUNT 6
#define TOP_SECRET_THREAD_COUNT 6

typedef struct Job {
	unsigned int jobNumber;
	int inCluster; //Stores the cluster that the job is in -1 if not
	SecurityLevel level;
	pthread_mutex_t threadLock;
	pthread_t jobThread;
	int isTail;
	struct Job *nextJob;
} Job;

//This is declared here because of circular dependency
#include "ClusterManager.h"

int initJobStuff();

int removeFirstJob(Job ** returnJob);

int removeFirstJobType(Job **returnJob, SecurityLevel level);

int removeSimilarJob(Job **returnJob, SecurityLevel level);

void addJobToList(Job *job);

void getJobListCountStats(int *us_count, int *s_count, int *ts_count);

void *jobThreadMethod(void *input);

Bool isJobInQueue();

int initJobStruct(Job *job, SecurityLevel level);

void initTheseJobs(SecurityLevel level);

void printJobListStats();

int isStarving();

void getFirstJob();

int runOnce;
int countFIQ;
int firstIQ; //store the ID of the first job in queue
int firstLevel; //store the level of firstIQ

#endif //_JOBS_H_
