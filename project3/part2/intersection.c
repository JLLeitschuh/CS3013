#include <semaphore.h>
#include <stdio.h>
#include "Vehicles.h"
#include "Boolean.h"
#include "Intersection.h"
#include "Error.h"

int main(int argc, char *argv[]){
  initVehicleStuff();
  initIntersectionManager();
  for(int i = 0; i < 20; i++){
    Vehicle *initVehicle = malloc(sizeof(Vehicle));
    initVehicleStruct(initVehicle, CAR);
  }
  Vehicle *emergencyVehicle = malloc(sizeof(Vehicle));
  initVehicleStruct(emergencyVehicle, EMERGENCY);

  usleep(3000);
  getchar();
  printVehicleQueueData();
  getchar();
  while(1){};
}
