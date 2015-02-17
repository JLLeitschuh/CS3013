#ifndef _CAR_H_
#define _CAR_H_

typedef struct car{
	pthread_t thread;
	char from; //N, S, E, W
	char turn; //LEFT, RIGHT, STR8
	int num;
	struct car* next;
} car;

void init_car(void);
void add_car(car* head, car* add);
car* get_car(car* head);
void car_control(void);

#endif //_CAR_H_
