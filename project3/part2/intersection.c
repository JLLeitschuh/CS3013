#include <semaphore.h>
#include "Vehicles.h"
#include "Boolean.h"
#include "Intersection.h"
#include "IntersectionManager.h"
#include "Error.h"

int main(int argc, char *argv[]){
  initVehicleStuff();
  initIntersectionManager();
  for(int i = 0; i < 20; i++){
    Vehicle *initVehicle = malloc(sizeof(Vehicle));
    initVehicleStruct(initVehicle, CAR);
  }
  while(1){
    usleep(3000);
  };
}
