/*
 * Project 1 - Phase 3
 * Phase 1 - Completed
 * Phase 2 - Completed
 * Phase 3 - Completed
 * Peerapat Luxsuwong - pluxsuwong
 * Jonathan Leitschuh - jlleitschuh
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define FALSE 0
#define TRUE 1
#define EMPTY 0
#define FULL 1

typedef struct process {
	long processNum;
	char* command;
	struct process *next;
	struct timeval* startT;
	pid_t pid;
} process;

void printstats(struct rusage);
void recprocess(struct process*, struct process*);
struct process* latestchild(struct process*);
struct process* remprocess(struct process*, pid_t);
struct process* findb4process(struct process*, pid_t);
struct process* logchild(long, char*, int, struct timeval, pid_t);
void printchildren(struct process*);
int childexists(struct process*, pid_t);
void checkchildren(struct process*);

int main(int argc, char* argv[]) {
	int i = 0;
	long taskNum = 0;
	process* shell2 = malloc(sizeof(process));
	shell2->processNum = 0;
	shell2->command = NULL;
	shell2->next = NULL;
	shell2->startT = malloc(sizeof(struct timeval));
	gettimeofday(shell2->startT, NULL);
	shell2->pid = -1;
	char pipe[128];
	int pipeFlag = EMPTY;
	
	/* Stats on Previous Child */
	long oldNivcsw = 0;
	long oldNvcsw = 0;
	long oldMajflt = 0;
	long oldMinflt = 0;
	double oldUtime = 0;
	double oldStime = 0;
	
	/* Initialize Pipeline */
	for (i = 0; i < 128; i++) {
		pipe[i] = 0;
	}
	
	while(TRUE) {
		const char delim[2] = " ";
		char bufIn[500];
		char* EOFPtr = NULL;
		char* token;
		char* tokIn[33];
		char** tokIn1 = &tokIn[0];
		int BGFlag = FALSE;
		int isValidInput = TRUE;
					
		/* Wipe Input Buffer */
		for (i = 0; i < 500; i++) {
			bufIn[i] = 0;
		}
		
		/* No Pipelined Instructions */
		if (pipeFlag == EMPTY) {
			
			/* Print Prompt */
			printf("==> ");
			
			/* Read Input from Stream to Buffer */
			EOFPtr = fgets(bufIn, 500, stdin);
			
			if (EOFPtr == NULL) {
				printf("\n");
				exit(1);
			}
			
			/* Check Input Length */
			for (i = 128; i < 500; i++) {
				if (bufIn[i] != 0) {
					isValidInput = FALSE;
				}
			}
			
			if (!isValidInput) {
				printf("error : input exceeds 128 chars\n");
				continue;			
			}
		}
		
		/* Execute Pipelined Instructions */
		else {
			memcpy(bufIn, &pipe[0], 128);
			
			/* Clean the Pipe after use */
			for (i = 0; i < 128; i++) {
				pipe[i] = 0;
			}
			
			pipeFlag = EMPTY;
			
		}
			
		/* Split Input into Tokens */
		for (i = 0; i < 128; i++) {
						
			if (bufIn[i] == '\n') {
				bufIn[i] = '\0';
				break;
			}
			
			/* Run Process in BG */
			if (bufIn[i] == '&' && bufIn[i + 1] != '&' && bufIn[i - 1] != '&') {
				BGFlag = TRUE;
				bufIn[i] = '\0';
			}
			
			/* Clear Rest of Buffer */
			else if (pipeFlag == FULL) {
				bufIn[i] = 0;
			}
			
			/* Copy Pipelined Commands to Persistent Buffer */
			else if (bufIn[i] == '&' && bufIn[i + 1] == '&' && pipeFlag == EMPTY) {
				int j = 0;
				
				/* Clean the Pipe before use */
				for (j = 0; j < 128; j++) {
					pipe[j] = 0;
				}

				memcpy(pipe, &bufIn[i + 2], 126 - i);
				
				bufIn[i] = '\0';
				bufIn[i + 1] = '\0';
				pipeFlag = FULL;
				
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
		
		/* New Line Nonsense */
		if (tokIn[0] == '\0') {
			checkchildren(shell2);
			
			// Do nothing!
		}
		
		/* Exit */
		else if (strncmp(tokIn[0], "exit", 4) == 0) {
			checkchildren(shell2);
			
			if (shell2->next == NULL) {
				exit(1);
			}
			else {
				printf("error : processes still running in background\n");
			}
		}
		
		/* Change Directory */
		else if (strncmp(tokIn[0], "cd", 2) == 0) {
			checkchildren(shell2);
			
			/* Invalid Directory */
			if (chdir(tokIn[1]) != 0) {
				printf("error : invalid directory\n");
			}
			
		}
		
		/* Print Background Processes */
		else if (strncmp(tokIn[0], "jobs", 4) == 0) {
			checkchildren(shell2);
			
			if (shell2->next != NULL) {
				printchildren(shell2->next);
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
				printf("error : unable to fork\n");
					
				exit(1);
			}
			
			/* In Child Process */
			else if (childPid == 0) {
				
				/* Execute Command */
				execvp(tokIn[0], tokIn1);					
				
				/* Execvp Error */
				printf("error : invalid argument(s)\n");
				
				exit(1);
			}
			
			/* In Parent Process */
			else {
				process* deadProcess;
				
				/* Not a Background Process */
				if (BGFlag == FALSE) {
					pid_t deadChildPid = 1;
					
					/* Wait for Child Process to Exit */
					/* Also Collect all Dead Children */
					while (TRUE) {
						int status = 0;
						struct rusage usage;
						deadChildPid = 1;
						
						deadChildPid = wait3(&status, 0, &usage);
						
						/* If there be Dead Children */
						if (deadChildPid > 0) {
							
							/* Dead Child Ran in BG */
							if (childexists(shell2, deadChildPid) == FALSE) {
								/* Display Stats */
								gettimeofday(&endWallClk, NULL);
								printf("Wall-Clock Time: %.3f ms\n", 
								((endWallClk.tv_sec*1000000 + endWallClk.tv_usec) 
								- (startWallClk.tv_sec*1000000 + startWallClk.tv_usec))*0.001);
								printstats(usage);
								break;
							}
							
							/* Dead Child was Non-BG */
							else {
								/* Indicate Completion of Background Task */
								deadProcess = remprocess(shell2, deadChildPid);
								printf("[%ld] %d %s completed.\n", deadProcess->processNum, deadChildPid, deadProcess->command);
								free(deadProcess);
								
								/* Display Stats */
								gettimeofday(&endWallClk, NULL);
								printf("Wall-Clock Time: %.3f ms\n", 
								((endWallClk.tv_sec*1000000 + endWallClk.tv_usec) 
								- (deadProcess->startT->tv_sec*1000000 + deadProcess->startT->tv_usec))*0.001);
								printstats(usage);
							}

						}

					}
					
				}
				
				/* New Background Process */
				else {
					process* newChild = NULL;
					
					/* Increment Incomplete Task Counter */
					taskNum++;
					
					/* Create Log of New Child Process */
					newChild = logchild(taskNum, tokIn[0], sizeof(tokIn[0]), startWallClk, childPid);
					
					/* Record New Background Child Process */
					recprocess(shell2, newChild);
			
					/* Indicate Background Task */
					printf("[%ld] %d\n", taskNum, childPid);
				}

			}

		}
		
	}
	
	return 0;
}

/* Prints out Stats of a Process */
void printstats(struct rusage usage) {
	double Utime, Stime;
	struct timeval UCPU, SCPU;	

	/* Get Usage Stats */
	UCPU = usage.ru_utime;
	Utime = (UCPU.tv_sec*1000000 + UCPU.tv_usec)*0.001;
	SCPU = usage.ru_stime;
	Stime = (SCPU.tv_sec*1000000 + SCPU.tv_usec)*0.001;


	/* Display Usage Stats */
	printf("User CPU Time Used: %.3f ms\n", Utime);
	printf("System CPU Time Used: %.3f ms\n", Stime);
	printf("Involuntary Preemptive Proceses: %ld\n", usage.ru_nivcsw);
	printf("Voluntary CPU Giveups: %ld\n", usage.ru_nvcsw);
	printf("Page Faults: %ld\n", usage.ru_majflt);
	printf("Page Reclaims: %ld\n", usage.ru_minflt);

}

/* Record Background Process */
void recprocess(process* shell2, process* newProcess) {
	if (shell2->next == NULL) {
		shell2->next = newProcess;
	}
	else {
		latestchild(shell2)->next = newProcess;
	}
}

/* 
 * Find Latest BG Process 
 * Helper Function of recprocess
 */
process* latestchild(process* aProcess) {
	if (aProcess->next->next == NULL) {
		return aProcess->next;
	}
	else {
		return latestchild(aProcess->next);
	}
}

/* Remove and Return Process */
process* remprocess(process* shell2, pid_t pid) {
	process* deadChild;
	process* b4DeadChildPtr;
	
	if (shell2->next != NULL) {
		if (shell2->next->pid == pid) {
			deadChild = shell2->next;
			shell2->next = deadChild->next;
			return deadChild;
		}
		else {
			b4DeadChildPtr = findb4process(shell2->next, pid);
			deadChild = b4DeadChildPtr->next;
			b4DeadChildPtr->next = deadChild->next;
			return deadChild;
		}
	}
	else {
		printf("There are no processes currently running in the background.\n");
		exit(1);
	}
}

/* Find the Process b4 a Specific Process */
process* findb4process(process* aProcess, pid_t pid) {
	if (aProcess->next != NULL) {
		if (aProcess->next->pid == pid) {
			return aProcess;
		}
		else if (aProcess->next->next != NULL) {
			findb4process(aProcess->next->next, pid);
		}
		else {
			return NULL;
		}
	}
	else {
		return NULL;
	}
}

/* Create Log of New Child Process */
process* logchild(long processNum, char* command, int cmdSize, struct timeval startTime, pid_t pid) {
	int i;
	
	process* newProcess = malloc(sizeof(process));
	newProcess->command = malloc(cmdSize + 1);
	newProcess->processNum = processNum;
	for (i = 0; i < cmdSize + 1; i++) {
		(newProcess->command)[i] = command[i];
	}
	newProcess->next = NULL;
	newProcess->startT = malloc(sizeof(struct timeval));
	newProcess->startT->tv_sec = startTime.tv_sec;
	newProcess->startT->tv_usec = startTime.tv_usec;
	newProcess->pid = pid;
	
	return newProcess;
}

/* Print Log of Alive Children */
void printchildren(process* aProcess) {
	printf("[%ld] %d %s\n", aProcess->processNum, aProcess->pid, aProcess->command);
	if (aProcess->next != NULL) {
		printchildren(aProcess->next);
	}
}

/* Find if Job exists in Log */
int childexists(process* aProcess, pid_t pid) {
	if (aProcess->pid == pid) {
		return 1;
	}
	else if (aProcess->next != NULL) {
		childexists(aProcess->next, pid);
	}
	else {
		return 0;
	}
}

/* Check for Dead Children */
void checkchildren(process* shell2) {
	struct timeval endWallClk;
	process* deadProcess;
	pid_t deadChildPid = 1;
	
	while (deadChildPid > 0) {
		struct rusage usage;
		int status = 0;
		deadChildPid = 1;
		deadChildPid = wait3(&status, WNOHANG, &usage);
		if (deadChildPid > 0) {
			/* Indicate Completion of Background Task */
			deadProcess = remprocess(shell2, deadChildPid);
			printf("[%ld] %d %s completed.\n", deadProcess->processNum, deadChildPid, deadProcess->command);
			free(deadProcess);
			
			/* Display Stats */
			gettimeofday(&endWallClk, NULL);
			printf("Wall-Clock Time: %.3f ms\n", 
				((endWallClk.tv_sec*1000000 + endWallClk.tv_usec) 
				- (deadProcess->startT->tv_sec*1000000 + deadProcess->startT->tv_usec))*0.001);
			printstats(usage);
		}
	}
}
