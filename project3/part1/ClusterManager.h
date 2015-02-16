#ifndef _CLUSTER_MANAGER_H_
#define _CLUSTER_MANAGER_H_

#include "SecurityLevel.h"
#include "Boolean.h"
#include "Jobs.h"

int initClusterStuff();

void manageCluster();

void exitCluster(Job *job);

#endif //_CLUSTER_MANAGER_H_
