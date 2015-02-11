#include <pthread.h>
#include <stdio.h>
#include "secOS.h"

void *secretThread(void *input){
  printf("Running secret thread\n");
}


int main(){
  sem_init (doorsign, 0, 3);

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

  void* individual(void* arg)
  {
    job* me = (job*)(arg);

    while(1)
    {
      int running = rand();
      int hold_it = rand();
      if (running <= 0)
      {
        running = -running + 1;
      }
      if (hold_it <= 0)
      {
        hold_it = -hold_it + 1;
      }

      usleep(hold_it);

      enter(me -> g); 
      usleep(running);
      leave();
      trips_count++;
    }

    printf("\n╔═══════════════\n║id: %i\n║sex: %s\n║\tstay:%i\n╚═══════════════\n", 
      me -> id, me -> p, me -> stay);

    free(me);
  }

  int main(int argc, char** argv)
  {
    srand(time(NULL));

    pthread_mutex_init(&doorsign, NULL);
    printf("initialized\n");

    for (i = 0; i < 20; i++)
    {
      job* i = (job*)(malloc(sizeof(job)));

      if (i == (13||11||18||4||19||7||16||2)) i-> p = UNCLASSIFIED;
      else if (i == (12||5||3||20||9||15) i -> p = TOP_SECRET;
        else i-> p = SECRET;

        i -> id = random();

        switch ( i -> p ) {
          case SECRET:
          i-> stay = 2;
          break;
          case TOP_SECRET:
          i->stay = 3;
          break;
          default:
          i->stay =1;
          break;
        }
      }
    // pthread_create(all_threads+arg_n_users, NULL, individual, i);
    }

    while(backup_n_user >= 0)
    {
      pthread_join(*(all_threads+backup_n_user), NULL);
      backup_n_user --;
    }

    finalize();
    return 0;
  }

  return 0;
}
