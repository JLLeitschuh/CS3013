/*
 * Project 1 - Phase 2
 * Phase 1 - Completed
 * Phase 2 - Completed
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
	/* Stats on Previous Child */
	long oldNivcsw = 0;
	long oldNvcsw = 0;
	long oldMajflt = 0;
	long oldMinflt = 0;
	double oldUtime = 0;
	double oldStime = 0;
	
	while(1) {
		char* EOFPtr;
		char* token;
		char delim[2] = " ";
		char bufIn[128];
		char* tokIn[33];
		char** tokIn1 = &tokIn[0];
		
		/* Print Prompt */
		printf("==> ");
		
		/* Read Input from Stream to Buffer */
		EOFPtr = fgets(bufIn, 128, stdin);
		
		/* Split Input into Tokens */
		int i;
		for (i = 0; i < 128; i++) {
			if (bufIn[i] == '\n') {
				bufIn[i] = '\0';
				break;
			}
		}
		
		token = (char*) strtok(bufIn, delim);
		
		for (i = 0; i < 33; i++) {
			while (token != NULL) {
				tokIn[i] = token;
				i++;
				token = (char*) strtok(NULL, delim);
			}
			tokIn[i] = NULL;
		}
		
		/* EOF */
		if (EOFPtr == NULL) {
			printf("\n");
			exit(1);
		}
		
		/* New Line Nonsense */
		else if (tokIn[0] == '\0') {
			// Do nothing!
		}
		
		/* Exit */
		else if (strncmp(tokIn[0], "exit", 4) == 0 || bufIn == NULL) {
			exit(1);
		}
		
		/* Change Directory */
		else if (strncmp(tokIn[0], "cd", 2) == 0) {
			
			/* Invalid Directory */
			if (chdir(tokIn[1]) != 0) {
				printf("error: invalid directory\n");
			}
			
		}
		
		/* Not Build-In Command */
		else {
			struct timeval startWallClk, endWallClk;
			
			/* Get Start Time */
			gettimeofday(&startWallClk, NULL);
			
			/* Fork off Child Process */
			pid_t childPid = fork();
			
			/* Check for Forking Error */
			if (childPid == -1) {
				printf("error: unable to fork\n");
					
				exit(1);
			}
			
			/* In Child Process */
			else if (childPid == 0) {
				
				/* Execute Command */
				execvp(tokIn[0], tokIn1);
				
				/* Execvp Error */
				printf("error: invalid argument(s)\n");
				
				exit(1);
			}
			
			/* In Parent Process */
			else {
				int status;
				double newUtime, newStime;
				struct rusage usage;
				struct timeval UCPU, SCPU;
				
				/* Wait for Child to Exit */
				waitpid(-1, &status, 0);
				
				/* Get Usage Stats */
				getrusage(RUSAGE_CHILDREN, &usage);
				UCPU = usage.ru_utime;
				newUtime = (UCPU.tv_sec*1000000 + UCPU.tv_usec)*0.001;
				SCPU = usage.ru_stime;
				newStime = (SCPU.tv_sec*1000000 + SCPU.tv_usec)*0.001;
				
				/* Get End Time */
				gettimeofday(&endWallClk, NULL);
				printf("Wall-Clock Time: %.3f ms\n", 
						((endWallClk.tv_sec*1000000 + endWallClk.tv_usec) 
						- (startWallClk.tv_sec*1000000 + startWallClk.tv_usec))*0.001);

				/* Display Usage Stats */
				printf("User CPU Time Used: %.3f ms\n",
						newUtime - oldUtime);
				printf("System CPU Time Used: %.3f ms\n",
						newStime - oldStime);
				printf("Involuntary Preemptive Proceses: %ld\n", usage.ru_nivcsw - oldNivcsw);
				printf("Voluntary CPU Giveups: %ld\n", usage.ru_nvcsw - oldNvcsw);
				printf("Page Faults: %ld\n", usage.ru_majflt - oldMajflt);
				printf("Page Reclaims: %ld\n", usage.ru_minflt - oldMinflt);
				
				/* Store Most Recent Child's Stats */
				oldUtime = newUtime;
				oldStime = newStime;
				oldNivcsw = usage.ru_nivcsw;
				oldNvcsw = usage.ru_nvcsw;
				oldMajflt = usage.ru_majflt;
				oldMinflt = usage.ru_minflt;
				
			}
		}
	}
	
	return 0;
}
