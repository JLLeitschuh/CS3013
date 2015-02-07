/*
CS 3013 Project 2 Part 2
Team 48
Alexandra Bittle - albittle
Jonathan Leitschuh - jlleitchuh
Long Nguyen - lhnguyen

Smite.c - this file handles the user side implementation for smite
*/


#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#define __NR_cs3013_syscall1 355
#define __NR_cs3013_syscall2 356
#define __NR_cs3013_syscall3 357

#define MAX_PROCESS_COUNT 100


/* smite user - function call which runs the kernel space smite code, checking to see if it ran correctly 
   or not.  */ 
void smiteUser(unsigned short target_uid, int *num_pids_smited, int *smited_pids, long *pid_states){
  errno = 0;
  if (syscall(__NR_cs3013_syscall2, &target_uid, num_pids_smited, smited_pids, pid_states) == -1) {
    int errsv = errno;
    printf("syscall(__NR_cs3013_syscall2) failed with errno: %i\n", errsv);
    return;
  }

}


/* main - This method reads in the target uid, calls the kernel space smiteUser code, and then prints the
   resulting information */
int main(int argc, char* argv[]){
  //Thich check on argc is to make sure a target uid has been input by the user
  if(argc == 1){
    printf("Error, Invalid number of arguments\n");
    return 1;
  } else if(argc > 2){
    printf("Error, Too many arguments\n");
  }

  char * pEnd;
  errno = 0;

  //Convert input string to integer value
  unsigned short target_uid = strtol(argv[1],&pEnd,10);
  int errsv = errno;
  if(errsv != 0){
    printf("Errno returned: %i\n", errsv);
  }

  //initualize information for use in smiteUser
  int num_pids_smited = 0;
  int smited_ids[MAX_PROCESS_COUNT];
  long pid_states[MAX_PROCESS_COUNT];

  smiteUser(target_uid, &num_pids_smited, smited_ids, pid_states);

  //iterate through the smited_pids and pid_states array and print the necessary information for use in unsmite
  int i;
  for(i = 0; i < num_pids_smited; i++){
    printf("%i %lu\n", smited_ids[i], pid_states[i]);
  }
}
