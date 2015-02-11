#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <time.h>
extern clusterFuck.c

pthread_mutex_t doorsign;

int occupancy = 0; //must < MAXIN (2)
priority occupant = UNCLASSIFIED; 
int inside;

#define MAXIN = 2;

void enter(priority p)
{
	while(1)
	{
		pthread_mutex_lock(&doorsign);
		if (p == UNCLASSIFIED)
		if (occupancy == 0 || occupant == p)
		{
			occupancy++;
			occupant = p;

			pthread_mutex_unlock(&doorsign);
			return;
		}

		pthread_mutex_unlock(&doorsign);
	}
}

void leave()
{
	pthread_mutex_lock(&doorsign);
	occupancy --;
	if (occupancy <= 0)
	{
		occupancy = 0;
	}

	pthread_mutex_unlock(&doorsign);
}


