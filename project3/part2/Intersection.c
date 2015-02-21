#include "Intersection.h"
#include "Vehicles.h"

sem_t intersectionLock;
IntersectionQuadrant NE_quad;
IntersectionQuadrant SE_quad;
IntersectionQuadrant SW_quad;
IntersectionQuadrant NW_quad;

void initIntersectionManager(){
  sem_init(&intersectionLock, 0 , 1);

  NE_quad.isOccupied = false;
  NE_quad.quadrant = NORTH_EAST;
  NE_quad.entry = EAST;
  NE_quad.nextQuadrant = &NW_quad;

  NW_quad.isOccupied = false;
  NW_quad.quadrant = NORTH_WEST;
  NW_quad.entry = NORTH;
  NW_quad.nextQuadrant = &SW_quad;

  SW_quad.isOccupied = false;
  SW_quad.quadrant = SOUTH_WEST;
  SW_quad.entry = WEST;
  SW_quad.nextQuadrant = &SE_quad;

  SE_quad.isOccupied = false;
  SE_quad.quadrant = SOUTH_EAST;
  SE_quad.entry = SOUTH;
  SE_quad.nextQuadrant = &NE_quad;
}

int getIntersectionQuadrantFromEntry(CardinalDirection entry, IntersectionQuadrant **returnQuad){
  switch(entry){
    case NORTH:
    *returnQuad = &NW_quad;
    break;
    case WEST:
    *returnQuad = &SW_quad;
    break;
    case SOUTH:
    *returnQuad = &SE_quad;
    break;
    case EAST:
    *returnQuad = &NE_quad;
    break;
    default:
    errorWithContext("Invaild entry");
    exit(1);
    break;
  }
  return 0;
}


/*
* Lock access to the intersection so that only one thread is interacting with it at a time
*/
void lockIntersection(){
  //printf("[SEMAPHORE] locked\n");
  sem_wait(&intersectionLock);
}//lhnguyen

/*
* Unlock access to the intersection so that another thread can go about modifying it
*/
void  unlockIntersection(){
  //printf("[SEMAPHORE] unlocked\n");
  sem_post(&intersectionLock);
}//lhnguyen

Bool isIntersectionAvailable(Vehicle *vehicle){
  IntersectionQuadrant *quadrant;
  if(getIntersectionQuadrantFromEntry(vehicle->entryPoint, &quadrant)){
    errorWithContext("You did something wrong");
    exit(1);
  }
  //Traverse the intersection to see if the route is free
  for(int i = 0; i < 3; i++){
    if(quadrant->isOccupied){
      return 0;
    }
    if(vehicle->destination == quadrant->quadrant){
      break;
    }
    quadrant = quadrant->nextQuadrant;
  }
  if(vehicle->destination != quadrant->quadrant){
    errorWithContext("The vehicle desitination does not equal the quadrant");
    exit(1);
  }
  return 1;
}

//return 1 is failure
int tryEnterIntersection(Vehicle *vehicle){
  lockIntersection();
  //printf("Trying to enter intersection\n");

  if(!isIntersectionAvailable(vehicle)){
    unlockIntersection();
    return 1;
  }

  IntersectionQuadrant *quadrant;
  if(getIntersectionQuadrantFromEntry(vehicle->entryPoint, &quadrant)){
    errorWithContext("You did something wrong");
    exit(1);
  }

  for(int i = 0; i < 3; i++){
    //printf("Iterating\n");
    if(quadrant->isOccupied){
      errorWithContext("The intersection is occuped but I'm claiming its resources");
      exit(1);
    }
    quadrant->isOccupied = true;
    if(vehicle->destination == quadrant->quadrant){
      break;
    }
    quadrant = quadrant->nextQuadrant;
  }
  unlockIntersection();
  return 0;
}

int moveThroughIntersection(Vehicle *vehicle){
  IntersectionQuadrant *quadrant;
  if(getIntersectionQuadrantFromEntry(vehicle->entryPoint, &quadrant)){
    errorWithContext("You did something wrong");
    exit(1);
  }

  for(int i = 0; i < 3; i++){
    if(!(quadrant->isOccupied)){
      errorWithContext("The intersection is not occupied but I've claimed the resource");
      exit(1);
    }
    quadrant->isOccupied = false;
    if(vehicle->destination == quadrant->quadrant){
      return 0;
    }
    quadrant = quadrant->nextQuadrant;
  }
}
