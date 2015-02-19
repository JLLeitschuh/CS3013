#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

#include "Boolean.h"
#include "Intersection.h"
#include <pthread.h>
#include <semaphore.h>

typedef enum {
  NORTH = 1,
  EAST = 0,
  SOUTH = 3,
  WEST = 2
} CardinalDirection;

typedef enum{
  NORTH_EAST = 0,
  SOUTH_EAST = 3,
  SOUTH_WEST = 2,
  NORTH_WEST = 1
} IntersectionQuadrant_t; // This is the type version

typedef struct IntersectionQuadrant{
  IntersectionQuadrant_t quadrant; //For easier comparisons
  CardinalDirection entry; //Where cars will enter from for this quadrant

  sem_t occupied; // If occupied then this will be less than or equal to 0 if not it will be a 1

  struct IntersectionQuadrant *nextQuadrant;
} IntersectionQuadrant;

#endif //_INTERSECTION_H_
