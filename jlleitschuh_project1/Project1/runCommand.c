/*
 * Project 1 - Checkpoint
 * Phase 1 - Completed
 * Peerapat Luxsuwong - pluxsuwong
 * Jonathan Leitschuh - jlleitschuh
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]) {
	
	while(1) {
		struct timeval startWallClk, endWallClk;
		/* Get Start Time */
		gettimeofday(&startWallClk, NULL);
		
		/* Fork off Child Process */
		pid_t childPid = fork();
		
		/* Check for Forking Error */
		if (childPid == -1) {
			printf("error: unable to fork\n");
			return 1;
		}
		
		/* In Child Process */
		else if (childPid == 0) {
			char** argv2 = &argv[1];
			
			/* Execute Command */
			execvp(argv[1], argv2);
			
			/* Execvp Error */
			printf("error: invalid argument(s)\n");
			return 1;
		}
		
		/* In Parent Process */
		else {
			int status;
			struct rusage usage;
			struct timeval UCPU, SCPU;
			
			/* Wait for Child to Exit */
			waitpid(-1, &status, 0);
			
			/* Get Usage Stats */
			getrusage(RUSAGE_CHILDREN, &usage);
			UCPU = usage.ru_utime;
			SCPU = usage.ru_stime;
			
			/* Get End Time */
			gettimeofday(&endWallClk, NULL);
			printf("Wall-Clock Time: %.3f ms\n", 
					((endWallClk.tv_sec*1000000 + endWallClk.tv_usec) 
					- (startWallClk.tv_sec*1000000 + startWallClk.tv_usec))*0.001);

			/* Display Usage Stats */
			printf("User CPU Time Used: %.3f ms\n",
					(UCPU.tv_sec*1000000 + UCPU.tv_usec)*0.001);
			printf("System CPU Time Used: %.3f ms\n",
					(SCPU.tv_sec*1000000 + SCPU.tv_usec)*0.001);
			printf("Involuntary Preemptive Proceses: %ld\n", usage.ru_nivcsw);
			printf("Voluntary CPU Giveups: %ld\n", usage.ru_nvcsw);
			printf("Page Faults: %ld\n", usage.ru_majflt);
			printf("Page Reclaims: %ld\n", usage.ru_minflt);
			
			return 1;
		}
	}
	
	return 0;
}
