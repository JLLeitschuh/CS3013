#ifndef _INTERSECTION_MANAGER_H_
#define _INTERSECTION_MANAGER_H_

#include <semaphore.h>
#include <limits.h>
#include "Vehicles.h"
#include "Boolean.h"
#include "Intersection.h"
#include "Random.h"

void initIntersectionManager();

void lockIntersection();

void unlockIntersection();


#endif //_INTERSECTION_MANAGER_H_
