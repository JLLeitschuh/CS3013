#include <pthread.h>
#include <stdio.h>

typedef enum {
  TOP_SECRET,
  SECRET,
  UNCLASSIFIED
} SecurityLevel;


void *secretThread(void *input){
    printf("Running secret thread\n");
}


int main(){

  pthread_t secretThread1;

  if(pthread_create(&secretThread1, NULL, secretThread, NULL)) {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }
  printf("Running main thread\n");

  if(pthread_join(secretThread1, NULL)) {
    fprintf(stderr, "Error joining thread\n");
    return 2;
  }

  return 0;
}
