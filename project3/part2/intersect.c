#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "intersect.h"
#include "car.h"


int main(int argc, char** argv) {
	srand(time(NULL));
	car_queue.next = NULL;

	//initiate all the semaphores
	sem_init(&road_nw, 0, 1);
	sem_init(&road_ne, 0, 1);
	sem_init(&road_sw, 0, 1);
	sem_init(&road_se, 0, 1);
	sem_init(&queue_lock, 0, 1);
	sem_init(&car_num, 0, 0);
	sem_init(&cars_waiting, 0, 0);

	pthread_t car_threads[MAX_NUM_CARS];
	pthread_t control_threads[3]; // three to avoid deadlock
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	int i;
	while(TRUE) {
		for(i = 0; i < MAX_NUM_CARS; i++){
			pthread_create(&car_threads[i], &attr, (void*)&init_car, NULL);
		}
		// start the control threads
		pthread_create(&control_threads[0], &attr, (void*)&car_control, NULL);
		pthread_create(&control_threads[1], &attr, (void*)&car_control, NULL);
		pthread_create(&control_threads[2], &attr, (void*)&car_control, NULL);
		pthread_join(control_threads[0], NULL);
		pthread_join(control_threads[1], NULL);
		pthread_join(control_threads[2], NULL);
	}
	return 0;
}

void init_car(void) {
	car* this_car = malloc(sizeof(car));
	this_car->thread = pthread_self();
	this_car->from = rand() % 4;
	this_car->turn = rand() % 3;

	sem_post(&car_num);
	sem_getvalue(&car_num, &(this_car->num));
	add_car(&car_queue, this_car);
	switch(this_car->from) {
		case NORTH:
		switch(this_car->turn) {
			case LEFT:
			printf("Car [%d] from NORTH, going EAST\n", this_car->num);
			break;
			case STRAIGHT:
			printf("Car [%d] from NORTH, going SOUTH\n", this_car->num);
			break;
			case RIGHT:
			printf("Car [%d] from NORTH, going WEST\n", this_car->num);
			break;
		}
		break;
		case EAST:
			//TODO
		case SOUTH:
			//TODO
		case WEST:
			//TODO
	}
}

void add_car(car* head, car* add) {
	sem_wait(&queue_lock);
	car* tail = head;
	while (tail->next != NULL) {
		tail = tail->next;
	}
	add->next = NULL;
	tail->next = add;
	sem_post(&cars_waiting);
	sem_post(&queue_lock);
}

car* get_car(car* head) {
//TODO
}

void car_control(void) {
	while (TRUE) {
		car* next_car = get_car(&car_queue);
		if (next_car != NULL) {
			switch(next_car->from) {
				case NORTH:
				switch(next_car->turn) {
					case LEFT:
					sem_wait(&road_nw);
					printf("Car [%d] entered NW\n", next_car->num);
					usleep(rand());
					sem_wait(&road_sw);
					printf("Car [%d] entered SW\n", next_car->num);
					sem_post(&road_nw);
					sem_wait(&road_se);
					printf("Car [%d] entered SE\n", next_car->num);
					sem_post(&road_sw);
					usleep(rand());
					printf("Car [%d] exited\n", next_car->num);
					sem_post(&road_se);
					break;
					case STRAIGHT:
							//TODO
					break;
					case RIGHT:
							//TODO
					break;
				}
				break;
				case EAST:
				case SOUTH:
				case WEST:
			}
		}
		else {
			free(next_car);
			pthread_exit(NULL);
		}
	}
}

