#include "IntersectionManager.h"

//Print helper functions
void printCarEntry(Vehicle* this_car , const char* message){
  IntersectionQuadrant_t destination = this_car->destination;
  if(this_car -> entryPoint == NORTH){
    if(destination == NORTH_WEST)
      printf("Car #%d %s right from the North going West\n", this_car -> vehicleNumber, message);
    else if(destination == SOUTH_WEST)
      printf("Car #%d %s straight from the North going South\n", this_car -> vehicleNumber, message);
    else if(destination == SOUTH_EAST)
      printf("Car #%d %s left from the North going East\n", this_car -> vehicleNumber, message);
  }
  else if(this_car -> entryPoint == SOUTH){
    if(destination == SOUTH_EAST)
      printf("Car #%d %s right from the South going East\n", this_car -> vehicleNumber, message);
    else if(destination == NORTH_EAST)
      printf("Car #%d %s straight from the South going North\n", this_car -> vehicleNumber, message);
    else if(destination == NORTH_WEST)
      printf("Car #%d %s left from the South going West\n", this_car -> vehicleNumber, message);
  }
  else if(this_car -> entryPoint == EAST){
    if(destination == NORTH_EAST)
      printf("Car #%d %s right from the East going North\n", this_car -> vehicleNumber, message);
    else if(destination == NORTH_WEST)
      printf("Car #%d %s straight from the East going West\n", this_car -> vehicleNumber, message);
    else if(destination == SOUTH_WEST)
      printf("Car #%d %s left from the East going South\n", this_car -> vehicleNumber, message);
  }
  else if(this_car -> entryPoint == WEST) {
    if(destination == SOUTH_WEST)
      printf("Car #%d %s right from the West going South\n", this_car -> vehicleNumber, message);
    else if(destination == SOUTH_EAST)
      printf("Car #%d %s straight from the West going East\n", this_car -> vehicleNumber, message);
    else if(destination == NORTH_EAST)
      printf("Car #%d %s left from the West going North\n", this_car -> vehicleNumber, message);
  }
} //lhnguyen

//The intersection lock that prevents multiple cars from moving through the intersection
sem_t intersectionLock;
sem_t entrySemaphore;
IntersectionQuadrant NE_quad;
IntersectionQuadrant SE_quad;
IntersectionQuadrant SW_quad;
IntersectionQuadrant NW_quad;

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

void initIntersectionManager(){
  sem_init(&intersectionLock, 0 , 1);
  sem_init(&entrySemaphore, 0 , 0);


  NE_quad.quadrant = NORTH_EAST;
  NE_quad.entry = EAST;
  NE_quad.nextQuadrant = &NW_quad;
  sem_init(&(NE_quad.occupied), 0 , 1);

  NW_quad.quadrant = NORTH_WEST;
  NW_quad.entry = NORTH;
  NW_quad.nextQuadrant = &SW_quad;
  sem_init(&(NW_quad.occupied), 0 , 1);

  SW_quad.quadrant = SOUTH_WEST;
  SW_quad.entry = WEST;
  SW_quad.nextQuadrant = &SE_quad;
  sem_init(&(SW_quad.occupied), 0 , 1);

  SE_quad.quadrant = SOUTH_EAST;
  SE_quad.entry = SOUTH;
  SE_quad.nextQuadrant = &NE_quad;
  sem_init(&(SE_quad.occupied), 0 , 1);
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


void enterIntersection(Vehicle *vehicle){
  //printVehicleStats(vehicle, "[CAR] Entering intersection\n");
  sem_wait(&(vehicle->currentQuadrant->occupied));
  printVehicleStats(vehicle, "Entering intersection");
  sem_post(&entrySemaphore);
}

/*
 * Blocks
 * This is run by the car threads as they try to move forward in the Intersection
 * return 0 if car has exited the intersection otherwise return 1
 */
int advanceMeForward(Vehicle *vehicle){
  //printf("[CAR] %d Advancing vehicle forward\n", vehicle->vehicleNumber);
  //Now unblocked
  IntersectionQuadrant *currentQuadrant = vehicle->currentQuadrant;
  IntersectionQuadrant *nextQuadrant = currentQuadrant->nextQuadrant;

  if(vehicle->currentQuadrant->quadrant == vehicle->destination) {
    //  then exit the intersection

    //  announce (printf) that you are leaving the intersection
    //printf("[CAR] Vehicle %d is leaving the intersection\n", vehicle->vehicleNumber);

    //  and unblock the quadrant

    //XXX: This may require an intersection lock arround it
    lockIntersection();
    printVehicleStats(vehicle, "Leaving intersection");
    sem_post(&(currentQuadrant->occupied));
    unlockIntersection();
    vehicle->currentQuadrant = NULL;

    return 0;
  }


  //printf("[CAR] %d Waiting on semaphore occupied for quadrant %d\n", vehicle->vehicleNumber, vehicle->currentQuadrant->quadrant);
  //blocks on the next intersection quadrant 'occupied' semaphore (vehicle should know what quadrant it's in)
  //lockIntersection();
  sem_wait(&(nextQuadrant->occupied));
  //unlockIntersection();

  //printVehicleStats(vehicle, "[CAR] After occupied, lock on intersection");
  //Now inside the next intersection quadrant
  lockIntersection(); //Do this here to prevent deadlocks
  //Now release the semaphore for the quadrant you were previously in.
  //printf("[CAR] %d After intersection lock, unlock previous\n", vehicle->vehicleNumber);
  sem_post(&(currentQuadrant->occupied));

  unlockIntersection(); //Do this here to prevent deadlocks

  //Update the vehicles "current intersection"
  vehicle->currentQuadrant = nextQuadrant;
   return 1;
}


/*
 * This method should not block
 * returns true if there are less than three cars in the intersection
 */
Bool isIntersectionAvailable(){
  //set the starting quadrant for counting
  IntersectionQuadrant *currentQuadrant = &NE_quad;
  int numOccupied = 0;

  //iterate through the intersection and count the number of quadrants which are occupied
  int i;
  for(i = 0; i < 4; i++);
  {
    int value;
    sem_getvalue(&(currentQuadrant->occupied), &value);
    if(value <= 1) numOccupied++;

    currentQuadrant = currentQuadrant->nextQuadrant;
  }

  //check to see if there are less than three cars in the intersection. if so, return true
  if(numOccupied < 3)
   return true;

  //if not, return false
  return false;
}//alex


/*
 * This method should not block
 * Figures out which quadrant we should pull in a car from.
 * Using this information it picks which car
 * Should return 0 if sucsessful and 1 if the quadrant that is open has no cars waiting in the queue.
 */
int getOptimalEntryPoint(CardinalDirection *entry){

  int numQuadsAccepting = 0;
  IntersectionQuadrant *currentQuad = &NE_quad;


  //If there are no quadrants available then exit(0) with an error because there should be a quadrant free
  if(!isIntersectionAvailable()){
    errorWithContext("There are no available quadrants");
    exit(1);
  }

  int shortest = INT_MAX;
  for(int i = 0; i < 4; i++){
     int value;
     sem_getvalue(&(currentQuad->occupied), &value);
     if(value >= 1) { //This quadrant is free

       IntersectionQuadrant_t thisDestination;
       // First checks what quadrants can currently accept cars.
       // Using this information it polls thoes car queues for where each car wants to go.
       if(getFirstDestination(currentQuad->entry, &thisDestination)){
         //Faiure so move on;
       } else {
         //Using this information we figure out which car has the shortest path to travel
         int distance = calculateTravelDistance(currentQuad->entry, thisDestination);
         if(shortest > distance){
           shortest = distance;
           //Store this information in the quadrant
           *entry = currentQuad->entry;
         }
       }
       currentQuad = currentQuad->nextQuadrant;
     }
  }
  //if(no cars in queue for all open quadrants) return 1;
  if(shortest == INT_MAX){ //the shortest was never set to a different value
    return 1;
  }

  return 0; //Sucsess
}//alex_unfin

/*
 * Should not block
 * Allows the given car access to the intersection
 */
void allowCarEntry(CardinalDirection entry){
  IntersectionQuadrant *quadrant;
  //printf("[Manager] allow Entry %d\n", entry);
  if(getIntersectionQuadrantFromEntry(entry, &quadrant)){
    errorWithContext("Invalid entry value");
    exit(1);
  }
  //Convert quadrant to CardinalDirection
  CardinalDirection entryDirection = quadrant->entry;
  //Get the first car in the queue for the given quadrant
  Vehicle *retrivedVehicle;
  if(removeFirstVehicle(entryDirection, &retrivedVehicle)){
    errorWithContext("Could not remove the first vehicle");
    exit(1);
  }
  //Unlock the cars queue mutex to allow it into the intersection
  retrivedVehicle->currentQuadrant = quadrant;
  //printVehicleStats(retrivedVehicle, "[MANAGER] Allowed to enter:");
  //Move the car into the intersection
  sem_post(&(retrivedVehicle->queueLock));
  int value;
  //printf("[MANAGER] Doing busywait\n");
  int overflowValue = 0;

  sem_wait(&entrySemaphore);

  //printf("[MANAGER] after allow car entry\n");
}

/*
 * Entry point for cars into the intersection.
 */
void manageIntersection(){
  //printf("[MANAGER] Begining Intersection management\n");
  while(1){
    //Freze cars where they are and dont let them change where they are in the intersection
    lockIntersection();
    //If there isn't space available in the intersection then dont procede
    if(!isIntersectionAvailable()){
      unlockIntersection();
      continue;
    }
    //printf("[MANAGER] Getting optimal entry\n");
    CardinalDirection optimalEntry;
    if(getOptimalEntryPoint(&optimalEntry)){
      /*
       * There is no optimal entry point.
       * This is either because the quadrants that are available don't have cars waiting to enter at them
       */
       unlockIntersection();
       continue;
    }
    //printf("[MANAGER] Car found\n");
    //We know which car has the optimal path
    //Allow the car that has the optimal entry path access to the intersection
    allowCarEntry(optimalEntry);
    unlockIntersection();
    //printf("[MANAGER] After car entry unlock\n");
  }
}


//determines the number of quadrants a vehicle must travel before exiting the intersection
int calculateTravelDistance(CardinalDirection from, IntersectionQuadrant_t dest){
  switch(from) {
    case NORTH:
      if(dest == NORTH_WEST) return 1;
      else if(dest == SOUTH_WEST) return 2;
      else if(dest == SOUTH_EAST) return 3;
      else if(dest == NORTH_EAST){
        errorWithContext("You attempted to U-turn. Not allowed");
        exit(0);
      } else {
        errorWithContext("Not a valid source direction");
        exit(0);
      }
      break;
    case WEST:
      if(dest == SOUTH_WEST)
        return 1;
      else if(dest == SOUTH_EAST)
        return 2;
      else if(dest == NORTH_EAST)
        return 3;
      else if(dest == NORTH_WEST) {
        errorWithContext("You attempted to U-turn. Not Allowed");
        exit(0);
      } else {
          errorWithContext("Not a valid source direction");
          exit(0);
      }
      break;
    case SOUTH:
      if(dest == SOUTH_EAST)
        return 1;
      else if(dest == NORTH_EAST)
        return 2;
      else if(dest == NORTH_WEST)
        return 3;
      else if(dest == SOUTH_WEST){
        errorWithContext("You attempted to U-turn. Not allowed");
        exit(0);
      } else {
        errorWithContext("Not a valid destination");
        exit(0);
      }
      break;
  case EAST:
    if(dest == NORTH_EAST)
      return 1;
    else if(dest == NORTH_WEST)
      return 2;
    else if(dest == SOUTH_WEST)
      return 3;
    else if(dest == SOUTH_EAST){
      errorWithContext("You attempted to U-turn. Not allowed");
      exit(0);
    } else {
      errorWithContext("Not a valid source direction");
      exit(0);
    }
    break;
  default:
    errorWithContext("Not a valid source direction");
    exit(0);
    break;
  }//end switch statement
  return -1; //return statement in case the current pair of dest and from has somehow ended up at this point
}//alex
