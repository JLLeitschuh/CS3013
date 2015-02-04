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



void smiteUser(unsigned short target_uid, int *num_pids_smited, int *smited_pids, long *pid_states){
  errno = 0;
  if (syscall(__NR_cs3013_syscall2, &target_uid, num_pids_smited, smited_pids, pid_states) == -1) {
    int errsv = errno;
    printf("syscall(__NR_cs3013_syscall2) failed with errno: %i\n", errsv);
    return;
  }

}

int main(int argc, char* argv[]){
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


  int num_pids_smited =0;
  int smited_ids[MAX_PROCESS_COUNT];
  long pid_states[MAX_PROCESS_COUNT];

  smiteUser(target_uid, &num_pids_smited, smited_ids, pid_states);

  int i;
  for(i = 0; i < num_pids_smited; i++){
    printf("%i %lu\n", smited_ids[i], pid_states[i]);
  }
}
