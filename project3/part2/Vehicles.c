#include "Vehicles.h"

sem_t N_list_lock;
Vehicle *N_list_start;
sem_t E_list_lock;
Vehicle *E_list_start;
sem_t S_list_lock;
Vehicle *S_list_start;
sem_t W_list_lock;
Vehicle *W_list_start;

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

void printVehicleStats(Vehicle *vehicle, char* additional_message){
  printf("[CAR] %35s Stats [Number: %2d Entry: %d, Destination: %d]\n", additional_message, vehicle->vehicleNumber, vehicle->entryPoint, vehicle->destination);
}

void wakeUp(CardinalDirection direction){
  Vehicle *head = _getVehicleListHead(direction);
  lockQueue(direction);
  if(!(head->isTail)){
    //Wake that thread up
    //printf("Wake up %d\n", head->nextVehicle->vehicleNumber);
    sem_post(&(head->nextVehicle->queueLock));
  }
  unlockQueue(direction);
}

void alertOtherCars(){
  wakeUp(NORTH);
  wakeUp(SOUTH);
  wakeUp(EAST);
  wakeUp(WEST);
}

int assignRandomPathToVehicle(Vehicle *vehicle){
  CardinalDirection entry = (CardinalDirection) getRandomBetween(0, 4);
  vehicle->entryPoint = entry;
  IntersectionQuadrant_t destination;
  do{ //Iterate until we find a desination that is valid for the given entry point
    destination = (IntersectionQuadrant_t) getRandomBetween(0,4);
  }while(!isValidPath(entry,destination));
  vehicle->destination = destination;
}


//Warning! This is not thread safe. Make sure you are using a lock!
void _addVehicle(Vehicle *toVehicle, Vehicle *addVehicle){
  if(toVehicle->isTail){
    //printf("Adding Vehicle\n");
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
  //printf("Pre lock\n");
  lockQueue(queueSelection);
  //printf("Adding Car %d to %d... \n", vehicle->vehicleNumber, queueSelection);
  _addVehicle(selectedVehicleHead, vehicle);
  //printf("Add complete\n");
  unlockQueue(queueSelection);
  //printf("Post lock\n");
}



/*
* Gets the first vehicle from the list and removes it.
* Return 1 if failure and 0 if sucsess
*/
int removeFirstVehicle(const CardinalDirection queueSelection, Vehicle **returnVehicle){
  Vehicle *selectedVehicleHead = _getVehicleListHead(queueSelection);
  int returnValue = 0;
  lockQueue(queueSelection);
  if(selectedVehicleHead->isTail){
    returnValue = 1;
  } else {
    *returnVehicle = selectedVehicleHead->nextVehicle;
    //If this is a one element list
    if(selectedVehicleHead->nextVehicle->isTail){
      //printf("is tail %d\n", queueSelection);
      selectedVehicleHead->nextVehicle = NULL;
      selectedVehicleHead->isTail = true;
    } else {
      selectedVehicleHead->nextVehicle = (*returnVehicle)->nextVehicle;
    }
    (*returnVehicle)->nextVehicle = NULL;
  }
  //printf("Removing Vehicle %d\n", (*returnVehicle)->vehicleNumber);
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
  lockQueue(queueSelection);
  if(selectedVehicleHead->isTail){
    returnValue = 1;
  } else {
    //I'm not sure if dereferencing pointers is automic so for the sake of safety
    *returnDestination = selectedVehicleHead->nextVehicle->destination;
  }
  unlockQueue(queueSelection);
  return 0;
}

int getFirstCarNumber(CardinalDirection queueSelection, int *number){
  Vehicle *selectedVehicleHead = _getVehicleListHead(queueSelection);
  int returnValue =0;
  lockQueue(queueSelection);
  if(selectedVehicleHead->isTail){
    returnValue = 1;
  } else {
    //I'm not sure if dereferencing pointers is automic so for the sake of safety
    *number = selectedVehicleHead->nextVehicle->vehicleNumber;
  }
  unlockQueue(queueSelection);
  return 0;
}

void _printVehicleQueueData(CardinalDirection queueSelection){
  Vehicle *selected = _getVehicleListHead(queueSelection);
  lockQueue(queueSelection);
  if(selected->isTail){
    printf("[QUEUE: %d] EMPTY\n", queueSelection);
  } else {
    int i =0;
    do {
      selected = selected->nextVehicle;
      printf("[QUEUE: %d] %d Number: %2d Entry: %d, Destination: %d\n", queueSelection, i, selected->vehicleNumber, selected->entryPoint, selected->destination);
      i++;
    } while(!selected->isTail);
    printf("\n");

  }
  unlockQueue(queueSelection);
}

void printVehicleQueueData(){
  _printVehicleQueueData(EAST);
  _printVehicleQueueData(NORTH);
  _printVehicleQueueData(WEST);
  _printVehicleQueueData(SOUTH);
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
void getQueueCount(int *N_count, int *E_count, int *S_count, int *W_count){

}

//The thread that runs each cars logic
void *carThread(void *input){ //jobthreadmethod //rmv
  Vehicle *this_vehicle = (Vehicle*) input;
  //Decrement the semaphore so that next time wait is called on it it blocks
  sem_wait(&(this_vehicle->queueLock));

  printf("[CAR] %d begining loop\n", this_vehicle->vehicleNumber);

  // Iterate infinitely
  while(1){
    //printf("[CAR] %d Assign car\n", this_vehicle->vehicleNumber);
    assignRandomPathToVehicle(this_vehicle);
    //printf("Add Vehicle to list\n");
    //enter queue for respective entry point

    printVehicleStats(this_vehicle, "Approaching intersection queue");
    addVehicleToList(this_vehicle->entryPoint, this_vehicle);

    //Check to see if I'm the first vehicle
    int number=-1;
    if(getFirstCarNumber(this_vehicle->entryPoint, &number)){
      errorWithContext("I added myself to the list but I'm not there... what happend?");
    }
    while(number != this_vehicle->vehicleNumber){
      //printVehicleStats(this_vehicle, "Number did not match, waiting on semaphore");
      if(sem_wait(&(this_vehicle->queueLock))==-1){
        errorWithContext("Error on semaphore wait");
        exit(1);
      }
      //printVehicleStats(this_vehicle, "I'm awake!");
      if(getFirstCarNumber(this_vehicle->entryPoint, &number)){
        errorWithContext("I shoud be in the list but no one is there what happend?");
      }
    }

    //Try to enter the intersection
    while(tryEnterIntersection(this_vehicle)){
      //Every time I fail go back to sleep

      if(sem_wait(&(this_vehicle->queueLock))){
        errorWithContext("Error on semaphore wait");
        exit(1);
      }
    }
    //printVehicleStats(this_vehicle, "I have conrol of the intersection");
    Vehicle *me;
    //printf("Running remove %d on queue: %d\n", this_vehicle->vehicleNumber, this_vehicle->entryPoint);
    //printVehicleQueueData();
    removeFirstVehicle(this_vehicle->entryPoint, &me);
    if(me->vehicleNumber != this_vehicle->vehicleNumber){
      printVehicleQueueData();
      printf("Number me: %d Number this:%d\n", me->vehicleNumber, this_vehicle->vehicleNumber);
      errorWithContext("I should be removing myself from the list but I'm not");
      exit(1);
    }
    printVehicleStats(this_vehicle, "Entering intersection");
    moveThroughIntersection(this_vehicle);
    printVehicleStats(this_vehicle, "Exiting intersection");

    /*
    * Tell the other cars that you are leaving and let them fight
    * for the intersection.
    */
    alertOtherCars();
  }
  return 0;
}//lhnguyen


int vehicleCount = 0;
int initVehicleStruct(Vehicle *init, VehicleLevel level){
  init->vehicleNumber = vehicleCount;
  vehicleCount++;
  init->level = level;
  init->isTail = true;
  init->nextVehicle = NULL;
  init->currentQuadrant = NULL;
  if(sem_init(&(init->queueLock), 0, 1)){
    errorWithContext("Semaphore init failed");
    exit(1);
  }
  switch(level){
    case CAR:
    if(pthread_create(&(init->vehicleThread), NULL, carThread, init)){
      errorWithContext("Thread creation failed");
      exit(1);
    }
    break;
    default:
    errorWithContext("Initialization not written for this vehicle type");
    exit(1);
    break;
  }

}

Bool isValidPath(CardinalDirection entry, IntersectionQuadrant_t destination){
  if(entry == NORTH && destination == NORTH_EAST ) return false;
  if(entry == EAST && destination == SOUTH_EAST) return false;
  if(entry == SOUTH && destination == SOUTH_WEST) return false;
  if(entry == WEST && destination == NORTH_WEST) return false;
  return true;
}

int _initVehicleListStuff(CardinalDirection direction){
  if(sem_init(_getListSemaphore(direction), 0 , 1)){
    errorWithContext("Failed to init list semaphore\n");
    exit(1);
  }
  Vehicle *initedVehicle = _getVehicleListHead(direction);
  initedVehicle->isTail = true;
  initedVehicle->vehicleNumber = -1;

  return 0;
}

int initVehicleStuff(){
  N_list_start = malloc(sizeof(Vehicle));
  E_list_start = malloc(sizeof(Vehicle));
  S_list_start = malloc(sizeof(Vehicle));
  W_list_start = malloc(sizeof(Vehicle));
  _initVehicleListStuff(NORTH);
  _initVehicleListStuff(SOUTH);
  _initVehicleListStuff(EAST);
  _initVehicleListStuff(WEST);
}
