#include "Vehicles.h"

sem_t N_list_lock;
Vehicle *N_list_start;
sem_t E_list_lock;
Vehicle *E_list_start;
sem_t S_list_lock;
Vehicle *S_list_start;
sem_t W_list_lock;
Vehicle *W_list_start;

Vehicle* _getVehicleListHead(CardinalDirection direction){
  switch(direction){
    case NORTH:
      return N_list_start;
      break;
    case SOUTH:
      return S_list_start;
      break;
    case EAST:
      return E_list_start;
      break;
    case WEST:
      return W_list_start;
      break;
    default:
      errorWithContext("Invaild cardinal direction selection");
      exit(1);
      break;
  }
  return NULL;
}

sem_t* _getListSemaphore(CardinalDirection direction){
  switch(direction){
    case NORTH:
      return &N_list_lock;
      break;
    case SOUTH:
      return &S_list_lock;
      break;
    case EAST:
      return &E_list_lock;
      break;
    case WEST:
      return &W_list_lock;
      break;
    default:
      errorWithContext("Invaild cardinal direction selection");
      exit(1);
    break;
  }
  return NULL;
}

void lockQueue(CardinalDirection direction){
  sem_wait(_getListSemaphore(direction));
}

void unlockQueue(CardinalDirection direction){
  sem_post(_getListSemaphore(direction));
}

int _initVehicleListStuff(CardinalDirection direction){
  if(sem_init(_getListSemaphore(direction), 0 , 1)){
    errorWithContext("Failed to init list semaphore\n");
    exit(1);
  }
  Vehicle *initedVehicle = _getVehicleListHead(direction);
  initedVehicle = malloc(sizeof(Vehicle));
  initedVehicle->isTail = true;
  initedVehicle->vehicleNumber = -1;

  return 0;
}

int initVehicleStuff(){
  _initVehicleListStuff(NORTH);
  _initVehicleListStuff(SOUTH);
  _initVehicleListStuff(EAST);
  _initVehicleListStuff(WEST);
}

//Warning! This is not thread safe. Make sure you are using a lock!
void _addVehicle(Vehicle *toVehicle, Vehicle *addVehicle){
  if(toVehicle->isTail){
    //printf("Adding Job\n");
    toVehicle->isTail = 0;
    addVehicle->isTail = 1;
    toVehicle->nextVehicle = addVehicle;
    return;
  } else {
    //printf("Recursing\n");
    _addVehicle(toVehicle->nextVehicle, addVehicle);
    return;
  }
}

void addVehicleToList(const CardinalDirection queueSelection, Vehicle *vehicle){
  Vehicle *selectedVehicleHead = _getVehicleListHead(queueSelection);
  lockQueue(queueSelection);
  _addVehicle(selectedVehicleHead, vehicle);
  unlockQueue(queueSelection);
}



/*
* Gets the first vehicle from the list and removes it.
* Return 1 if failure and 0 if sucsess
*/
int removeFirstVehicle(const CardinalDirection queueSelection, Vehicle **returnVehicle){
  Vehicle *selectedVehicleHead = _getVehicleListHead(queueSelection);
  int returnValue = 0;
  if(selectedVehicleHead->isTail) return 1;
  lockQueue(queueSelection);
  *returnVehicle = selectedVehicleHead->nextVehicle;
  if(selectedVehicleHead->nextVehicle->isTail){
    selectedVehicleHead->nextVehicle = NULL;
    selectedVehicleHead->isTail = true;
  } else {
    selectedVehicleHead->nextVehicle = (*returnVehicle)->nextVehicle;
    (*returnVehicle)->nextVehicle = NULL;
  }
  unlockQueue(queueSelection);
  return returnValue;
}


/*
* Gets the first vehicles destination
* returns 1 if there is no vehicle waiting 0 if sucsessful
*/
int getFirstDestination(CardinalDirection queueSelection, IntersectionQuadrant_t *returnDestination){
  Vehicle *selectedVehicleHead = _getVehicleListHead(queueSelection);
  int returnValue =0;
  if(selectedVehicleHead->isTail){
    return 1;
  }
  lockQueue(queueSelection);
  //I'm not sure if dereferencing pointers is automic so for the sake of safety
  *returnDestination = selectedVehicleHead->nextVehicle->destination;
  unlockQueue(queueSelection);
  return 0;
}

Bool areAnyVehiclesOfType(CardinalDirection queue, VehicleLevel level){

}


Bool isEmergencyVehicleWaiting(){

}

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
