#include <semaphore.h>
#include "Vehicles.h"
#include "Boolean.h"

//Print helper functions
void printCarEntry(*Vehicle this_car , const char* message){
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

void *carThread(void *input){ //jobthreadmethod //rmv
  Vehicle *this_vehicle = (Vehicle*) input;
  //Blocking so it won't run out of turn
  sem_wait(&(this_vehicle->threadLock));

  long randomWaitTime = getRandomBetween(500000, 2000000);
  printf("Car %d will delay for %ld us\n", this_vehicle->vehicleNumber, randomWaitTime);

  // Iterate infinitely
  while(1){
    //wait for some time before entering the queue
    usleep(randomWaitTime);
    //enter queue for respective entry point
    swith(this_vehicle->entryPoint)
    {
      case NORTH:
      addVehicleToList(&N_list_start, *this_vehicle);
      break;
      case SOUTH:
      addVehicleToList(&N_list_start, *this_vehicle);
      break;
      case EAST:
      addVehicleToList(&N_list_start, *this_vehicle);
      break;
      case WEST:
      addVehicleToList(&N_list_start, *this_vehicle);
      break;
    }

     //Block on queue semaphore
    sem_wait(this_vehicle->queueLock);
    //Now that I'm here I should be in the intersection

    //attempt to advance in the intersection until you leave
    while(advanceMeForward(this_vehicle)){
      //we have exited the intersection
      //Wait for a little bit before returing to the queue
      long randomRunTime = getRandomBetween(500000, 2000000);
      usleep(randomRunTime);
      randomWaitTime = getRandomBetween(4000000, 9000000);
    }
  }
  printf("Car %d crossing from %d has passed to %d\n", this_vehicle->vehicleNumber, this_vehicle->entryPoint, this_vehicle->destination );
  return 0;
}//lhnguyen

//The intersection lock that prevents multiple cars from moving through the intersection
sem_t intersectionLock;
IntersectionQuadrant NE_quad;
IntersectionQuadrant SE_quad;
IntersectionQuadrant SW_quad;
IntersectionQuadrant NW_quad;

void initIntersectionManager(){
  sem_init(&intersectionLock, 0 , 1);


  NE_quad.quadrant = NORTH_EAST;
  NE_quad.nextQuadrant = &NW_quad;
  sem_init(&(NE_quad.occupied), 0 , 1);

  NW_quad.quadrant = NORTH_WEST;
  NW_quad.nextQuadrant = &SW_quad;
  sem_init(&(NW_quad.occupied), 0 , 1);

  SW_quad.quadrant = SOUTH_WEST;
  SW_quad.nextQuadrant = &SE_quad;
  sem_init(&(SW_quad.occupied), 0 , 1);

  SE_quad.quadrant = SOUTH_EAST;
  SE_quad.nextQuadrant = &NE_quad;
  sem_init(&(SE_quad.occupied), 0 , 1);
}


/*
 * Lock access to the intersection so that only one thread is interacting with it at a time
 */
void lockIntersection(){
  sem_wait(intersectionLock);
}//lhnguyen

/*
 * Unlock access to the intersection so that another thread can go about modifying it
 */
void  unlockIntersection(){
  sem_post(intersectionLock);
}//lhnguyen

/*
 * Blocks
 * This is run by the car threads as they try to move forward in the Intersection
 * return 0 if car has exited the intersection otherwise return 1
 */
 int advanceMeForward(Vehicle *vehicle){
   //Now unblocked
   IntersectionQuadrant *currentQuadrant = vehicle->currentQuadrant;
   IntersectionQuadrant *nextQuadrant = currentQuadrant->nextQuadrant;

   //blocks on the next intersection quadrant 'occupied' semaphore (vehicle should know what quadrant it's in)
   sem_wait(&(nextQuadrant->occupied));


   //Now inside the next intersection quadrant
   lockIntersection(); //Do this here to prevent deadlocks
   //Now release the semaphore for the quadrant you were previously in.
   sem_post(&(currentQuadrant->occupied));

   unlockIntersection(); //Do this here to prevent deadlocks

   //Update the vehicles "current intersection"
   vehicle->currentQuadrant = nextQuadrant;

   //if(the vehicle is at its intended exit destination)
   if(vehicle->currentQuadrant->quadrant == vehicle->desination)
   {
     //  then exit the intersection
     // TODO

     //  announce (printf) that you are leaving the intersection
     printf("Vehicle is leaving the intersection");

     //  and unblock the quadrant
     sem_post(&(currentQuadrant->occupied));

     return 0;
   }

   return 1;
}


/*
 * This method should not block
 * returns true if there are less than three cars in the intersection
 */
Bool isIntersectionAvailable(){
  //set the starting quadrant for counting
  IntersectionQuadrant currentQuadrant = NE_quad;
  int numOccupied = 0;

  //iterate through the intersection and count the number of quadrants which are occupied
  int i;
  for(i = 0; i < 4; i++);
  {
    if(currentQuadrant->occupied == 1)
      numOccupied++;
    currentQuadrant = *(currentQuadrant.nextQuadrant);
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
int getOptimalEntryPoint(IntersectionQuadrant_t *quadrant){

  int numQuadsAccepting = 0;
  IntersectionQuadrant_t currentlyAccepting[4];
  IntersectionQuadrant currentQuad = NE_quad;


  //If there are no quadrants available then exit(0) with an error because there should be a quadrant free
  if(!isIntersectionAvailable){
    errorWithContext("There are no available quadrants");
    exit(0);
  }

  // First checks what quadrants can currently accept cars.
  int i;
  for(i = 0; i < 4; i++){
    if(currentQuad.occupied <= 0) {
      currentlyAccepting[numQuadsAccepting] = currentQuadrant.quadrant;
      numQuadsAccepting++;
    }
    currentQuad = *(currentQuad.nextQuadrant);
  }
  // Using this information it polls thoes car queues for where each car wants to go.

  //if(no cars in queue for all open quadrants) return 1;

  //Using this information we figure out which car has the shortest path to travel

  //If all have equal travel lengths then we pick one psudo randomly

  //Store this information in the quadrant
  return 0; //Sucsess
}//alex_unfin

/*
 * Should not block
 * Allows the given car access to the intersection
 */
void allowCarEntry(IntersectionQuadrant_t quadrant){
  //Convert quadrant to CardinalDirection
  CardinalDirection entryDirection;
  //Get the first car in the queue for the given quadrant
  Vehicle * retrivedVehicle;
  if(removeFirstVehicle(entryDirection, &retrivedVehicle)){
    errorWithContext("Error");
  }
  //Unlock the cars queue mutex to allow it into the intersection
  sem_post(&(retrivedVehicle->queueLock));
  //Move the car into the intersection
  while(->occupied == 1){
    //busy wait
  }
}

/*
 * Entry point for cars into the intersection.
 */
void manageIntersection(){
  while(1){
    //Freze cars where they are and dont let them change where they are in the intersection
    lockIntersection();
    //If there isn't space available in the intersection then dont procede
    if(!isIntersectionAvailable()){
      unlockIntersection();
      continue;
    }

    IntersectionQuadrant_t optimalEntry;
    if(getOptimalEntryPoint(&optimalEntry)){
      /*
       * There is no optimal entry point.
       * This is either because the quadrants that are available don't have cars waiting to enter at them
       */
       unlockIntersection();
       continue;
     }
    //We know which car has the optimal path
    //Allow the car that has the optimal entry path access to the intersection
     allowCarEntry(optimalEntry);
     unlockIntersection();
   }
 }


//determines the number of quadrants a vehicle must travel before exiting the intersection
int calculateTravelDistance(CardinalDirection from, intersectionQuadrant_t dest){

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
      break;l
  case EAST:
    if(dest == NORTH_EAST)
      return 1;
    else if(dest == NORTH_WEST)
      return 2;
    else if(dest == SOUTH_WEST)
      return 3;
    if(dest == SOUTH_EAST){
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
