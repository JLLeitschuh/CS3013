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

void unsmiteUser(int *num_pids_smited, int *smited_pids, long *pid_states)
{
	errno = 0;
	if(syscall(__NR_cs3013_syscall3, *num_pids_smited, *smited_pids, *pid_states) == -1){
		int errsv = errno;
		printf("syscall(__NR_cs3013_syscall3) failed with errno: %i\n", errsv);
		return;
	}

}

int main(int argc, char* argv[]){

	printf("running\n");
	int num_pids_smited = 0;
	int smited_pid[MAX_PROCESS_COUNT];
	int pid_states[MAX_PROCESS_COUNT];

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

		int pid;
		int state;
		sscanf(bufIn, "%d %d", &pid, &state); 

		smited_pid[num_pids_smited] = pid;
		pid_states[num_pids_smited] = state;
		num_pids_smited++;

	}
	int i;
	for(i = 0; i <  num_pids_smited; i++){
		printf("PID: %d STATE: %d\n", smited_pid[i], pid_states[i]);
	}

	unsmiteUser(&num_pids_smited, &smited_pids, &pid_states);

}
