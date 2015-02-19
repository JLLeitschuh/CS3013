#include "Vehicles.h"

sem_t N_list_lock;
Vehicle *N_list_start;
sem_t E_list_lock;
Vehicle *E_list_start;
sem_t S_list_lock;
Vehicle *S_list_start;
sem_t W_list_lock;
Vehicle *W_list_start;


void *carThread(void *input){ //jobthreadmethod //rmv
  Vehicle *this_vehicle = (Vehicle*) input;
  //Decrement the semaphore so that next time wait is called on it it blocks
  sem_wait(&(this_vehicle->queueLock));

  printf("[CAR] %d begining loop\n", this_vehicle->vehicleNumber);

  // Iterate infinitely
  while(1){
    printf("[CAR] %d Assign car\n", this_vehicle->vehicleNumber);
    assignRandomPathToVehicle(this_vehicle);
    //printf("Add Vehicle to list\n");
    //enter queue for respective entry point
    addVehicleToList(this_vehicle->entryPoint, this_vehicle);

    //Block on queue semaphore
    //printf("Before queue lock\n");
    sem_wait(&(this_vehicle->queueLock));
    printf("[CAR] After queue lock\n");
    //exit(1);
    enterIntersection(this_vehicle);
    //Now that I'm here I should be in the intersection

    //attempt to advance in the intersection until you leave
    while(advanceMeForward(this_vehicle)){
      printf("[CAR]looping\n");
    }

    //we have exited the intersection
    //Wait for a little bit before returing to the queue
  }
  printf("Car %d crossing from %d has passed to %d\n", this_vehicle->vehicleNumber, this_vehicle->entryPoint, this_vehicle->destination );
  return 0;
}//lhnguyen

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

int assignRandomPathToVehicle(Vehicle *vehicle){
  CardinalDirection entry = (CardinalDirection) getRandomBetween(0, 4);
  vehicle->entryPoint = entry;
  IntersectionQuadrant_t destination;
  do{ //Iterate until we find a desination that is valid for the given entry point
    destination = (IntersectionQuadrant_t) getRandomBetween(0,4);
  }while(!isValidPath(entry,destination));
  vehicle->destination = destination;
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
  //printf("Adding Car %d to %d... ", vehicle->vehicleNumber, queueSelection);
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

void _printVehicleQueueData(CardinalDirection queueSelection){
  Vehicle *selected = _getVehicleListHead(queueSelection);
  lockQueue(queueSelection);
  if(selected->isTail){
    printf("[QUEUE: %d] EMPTY\n", queueSelection);
  } else {
    do {
      selected = selected->nextVehicle;
      printf("[QUEUE: %d] Number: %2d Entry: %d, Destination: %d\n", queueSelection, selected->vehicleNumber, selected->entryPoint, selected->destination);
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
