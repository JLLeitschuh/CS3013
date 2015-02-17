#include <semaphore.h>

typedef enum{
  NORTH_EAST,
  SOUTH_EAST,
  SOUTH_WEST,
  NORTH_WEST
} IntersectionQuadrant_t; // This is the type version


void *carThread(void *input){
  Vehicle *vehicle = (Vehicle*) input;
  //Initialize stuff
  while(1){
    //enter queue for respective entry point
    //Block on queue semaphore

    //Now that I'm here I should be in the intersection

    //attempt to advance in the intersection until you leave
    while(advanceMeForward(vehicle)){}

    //Wait for a little bit before returing to the queue
  }
}


typedef struct IntersectionQuadrant{
  IntersectionQuadrant_t quadrant; //For easier comparisons
  sem_t occupied; // If occupied then this will be a 0 if not it will be a 1
  struct IntersectionQuadrant *nextQuadrant;
} IntersectionQuadrant;

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

}

/*
 * Unlock access to the intersection so that another thread can go about modifying it
 */
void unlockIntersection(){

}

/*
 * Blocks
 * This is run by the car threads as they try to move forward in the Intersection
 * return 0 if car has exited the intersection otherwise return 1
 */
int advanceMeForward(Vehicle *vehicle){
  //Now unblocked

  //blocks on the next intersection quadrant 'occupied' semaphore (vehicle should know what quadrant it's in)



  //Now inside the next intersection quadrant
  lockIntersection(); //Do this here to prevent deadlocks
  //Now release the semaphore for the quadrant you were previously in.

  unlockIntersection(); //Do this here to prevent deadlocks

  //Update the vehicles "current intersection"

  //if(the vehicle is at its intended exit destination)
  //  then exit the intersection
  //  announce (printf) that you are leaving the intersection
  //  and unblock the quadrant

  //return
}


/*
 * This method should not block
 * returns true if there are less than three cars in the intersection
 */
Bool isIntersectionAvailable(){
  //TODO
}


/*
 * This method should not block
 * Figures out which quadrant we should pull in a car from.
 * Using this information it picks which car
 * Should return 0 if sucsessful and 1 if the quadrant that is open has no cars waiting in the queue.
 */
int getOptimalEntryPoint(IntersectionQuadrant_t *quadrant){
  //If there are no quadrants available then exit(0) with an error because there should be a quadrant free

  // First checks what quadrants can currently accept cars.

  // Using this information it polls thoes car queues for where each car wants to go.

  //if(no cars in queue for all open quadrants) return 1;

  //Using this information we figure out which car has the shortest path to travel

  //If all have equal travel lengths then we pick one psudo randomly

  //Store this information in the quadrant
  return 0; //Sucsess
}

/*
 * Should not block
 * Allows the given car access to the intersection
 */
void allowCarEntry(IntersectionQuadrant_t quadrant){
  //Get the first car in the queue for the given quadrant
  //Move the car into the intersection
  //Unlock the cars queue mutex to allow it into the intersection
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
    allowCarEntry(optimalEntry));
    unlockIntersection();
  }
}
