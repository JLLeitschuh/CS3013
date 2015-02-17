#ifndef _VEHICLES_H_
#define _VEHICLES_H_

#include "Boolean.h"
#include <pthread.h>

typedef enum {
  CAR = 0,
  EMERGENCY=1,
  MOTORCADE=2,
  NONE =-1
} VehicleLevel;

typedef struct Vehicle {
  unsigned int vehicleNumber;
  VehicleLevel level;

  sem_t threadLock;
  pthread_t jobThread;


  Bool isTail;
  struct Vehicle *nextVehicle;
} Vehicle;

#endif
