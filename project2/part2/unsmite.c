/* CS 3013 Project 2 Part 2
Team 48
Alexandra Bittle - albittle
Jonathan Leitschuh - jlleitchuh
Long Nguyen - lhnguyen

unsmite.c - this code handles the userspace code for the implementation of unsmite
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

//unsmiteUser - function which passes the input information into the unsmite kernel call
void unsmiteUser(int num_pids_smited, int *smited_pids, long *pid_states)
{
	errno = 0;
	//Call the unsmite kernel call with the given information and set errno if any errors have occurred
	if(syscall(__NR_cs3013_syscall3, &num_pids_smited, smited_pids, pid_states) == -1){
		int errsv = errno;
		//If an error has occurred, print an error message
		printf("syscall(__NR_cs3013_syscall3) failed with errno: %i\n", errsv); 
		return;
	}

}

/* main - function which reads the output data from the output of calling smite, excecutes unsmite, and prints the 
   necessary information about the unsmited uids */
int main(int argc, char* argv[]){
	int num_pids_smited = 0;
	int smited_pid[MAX_PROCESS_COUNT];
	long pid_states[MAX_PROCESS_COUNT];

	//This while loop reads all pairs of information from the input file, whcih represent the changed pids and their previous state
	//It scans the input into a buffer and then saves it into smited_pid and pid_states
	while(1){
		char* EOFPtr = NULL;
		char bufIn[500];


		int i;
		for(i = 0; i< 500; i++){
			bufIn[i] = 0;
		}

		EOFPtr = fgets(bufIn, 500, stdin);

		if(EOFPtr == NULL)
		{
			break;
		}

		//Copy the input from the buffer into the designated arrays
		int pid;
		int state;
		sscanf(bufIn, "%d %d", &pid, &state);

		smited_pid[num_pids_smited] = pid;
		pid_states[num_pids_smited] = state;
		num_pids_smited++;
	}

	//iterate through the smited_pid and pid_states arrays and print the input data
	int i;
	for(i = 0; i <  num_pids_smited; i++){
		printf("PID: %d STATE: %ld\n", smited_pid[i], pid_states[i]);
	}

	//Call unsmiter user using the input data
	unsmiteUser(num_pids_smited, smited_pid, pid_states);

}
