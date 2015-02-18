#ifndef _INTERSECTION_H_
#define _INTERSECTION_H_

typedef enum {
  NORTH,
  EAST,
  SOUTH,
  WEST
} CardinalDirection;

typedef enum{
  NORTH_EAST,
  SOUTH_EAST,
  SOUTH_WEST,
  NORTH_WEST
} IntersectionQuadrant_t; // This is the type version

typedef struct IntersectionQuadrant{
  IntersectionQuadrant_t quadrant; //For easier comparisons

  sem_t occupied; // If occupied then this will be less than or equal to 0 if not it will be a 1

  struct IntersectionQuadrant *nextQuadrant;
} IntersectionQuadrant;

#endif //_INTERSECTION_H_
