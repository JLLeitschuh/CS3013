#ifndef _INTERSECT_H_
#define _INTERSECT_H_

#define TRUE 1
#define FALSE 0

#define MAX_NUM_CARS 10

#define LEFT 0
#define STRAIGHT 1
#define RIGHT 2

#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3

sem_t road_nw;
sem_t road_ne;
sem_t road_sw;
sem_t road_se;

sem_t car_num;
sem_t queue_lock;

sem_t cars_waiting;

// FIFO queue for all cars
car car_queue;

#endif //_INTERSECT_H_
