#ifndef _VEHICLES_H_
#define _VEHICLES_H_

#include "Boolean.h"
#include "Intersection.h"
#include <pthread.h>

typedef enum {
  CAR = 0,
  EMERGENCY=1,
  MOTORCADE=2,
  NONE =-1
} VehicleLevel;

typedef struct Vehicle {
  IntersectionQuadrant_t destination;
  CardinalDirection entryPoint;

  IntersectionQuadrant *currentQuadrant;

  unsigned int vehicleNumber; //The number of this vehicle
  VehicleLevel level; //The level that this job has

  sem_t queueLock; //The lock that prevents the Vehicle from entering the intersection
  pthread_t jobThread; // The pointer to the actual thread that is running


  Bool isTail; // Is this Vehicle the last element of the linked list
  struct Vehicle *nextVehicle; // Pointer to the next vehicle in the linked list
} Vehicle;


void addVehicleToList(CardinalDirection queueSelection, Vehicle *vehicle);

/*
 * Gets the first vehicle from the list and removes it.
 * Return 1 if failure and 0 if sucsess
 */
int removeFirstVehicle(CardinalDirection queueSelection, Vehicle **returnVehicle);


/*
 * Gets the first vechicles destination
 * returns 1 if there is no vehicle waiting 0 if sucsessful
 */
int getFirstDestination(CardinalDirection queueSelection, IntersectionQuadrant_t *returnDestination);


Bool isEmergencyVehicleWaiting();

/*
 * Gets the cardinal direction that the emergency vehcile will come from
 * return 1 if failure and 0 if sucsess
 */
int getEmergencyVehicleEntry(CardinalDirection *returnDirection);

Bool isMotorcadeWaiting();

int getMotorcadeEntry(CardinalDirection *returnDirection);

/*
 * Gets the count of how many cars are waiting in each respective queue.
 */
void getQueueCount(int *N_count, int *E_count, int *S_count, int *W_count);



#endif
