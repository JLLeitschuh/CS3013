#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/syscalls.h>
#include <asm/current.h>
#include <linux/list.h>
#include <linux/sched.h>
#include <asm/errno.h>
#include <asm/uaccess.h>

unsigned long **sys_call_table;

asmlinkage long (*ref_sys_cs3013_syscall2)(void);
asmlinkage long (*ref_sys_cs3013_syscall3)(void);

// *target_uid is a pointer to an unsigned short,
// *num_pids_smited is a pointer to an integer,
// *smited_pids is a reference to an integer array of size 100
// and *pid_states is a reference to a long array of size 100.

// Each of hese variables must have their memory allocated in user space before invoking the system call, otherwise an error will be returned.
// The system call returns zero if successful or an error indication if not successful.

asmlinkage long new_sys_cs3013_syscall2(unsigned short *target_uid, int *num_pids_smited, int *smited_pids, long *pid_states) {
	int 	ksmited_pids[100];
	long	kpid_states[100];

	int myUID = current_uid().val;  //get uid
	printk(KERN_INFO "Target Uid: %d\n", *target_uid);
	printk(KERN_INFO "My Uid: %d\n", myUID);

	if(target_uid == NULL){
		return -1;
	}

	if(num_pids_smited == NULL){
		return -1;
	}

	if(smited_pids == NULL){
		return -1;
	}

	if(pid_states == NULL){
		return -1;
	}

	if (*target_uid == myUID){
		printk(KERN_INFO "You can't smite yoursef!\n");
		return -1;
		// printk(KERN_INFO "Current UID = %d\n, Current PID = %d\n", myUID, current->pid);
		// ksmited_pids[knum_pids_smited] = current->pid;
		// pid_states[knum_pids_smited] = current->state;
		// knum_pids_smited++;
	} else if(*target_uid < 1000){
		printk(KERN_INFO "You can't smite a user with an id less than 1000 (root)!\n");
		return -1;
	}


	struct task_struct *tsk;
	int knum_pid_smitted = 0;
	for_each_process(tsk) {
		unsigned int uid_of_task = tsk->real_cred->uid.val;
		if(uid_of_task == *target_uid){
			printk(KERN_INFO "UID: %u PID: %d \n", uid_of_task, tsk->pid);
			ksmited_pids[knum_pid_smitted] = tsk->pid;
			kpid_states[knum_pid_smitted] = tsk->state;
			tsk->state = -1;
			knum_pid_smitted ++;
		}
	}

	if(copy_to_user(num_pids_smited, &knum_pid_smitted, sizeof(int))){
		return -EFAULT;
	}

	if (copy_to_user(smited_pids, ksmited_pids, sizeof(ksmited_pids))){
		return -EFAULT;
	}

	if (copy_to_user(pid_states, kpid_states, sizeof(kpid_states))){
		return -EFAULT;
	}

	return 0;
	//
  //   //Copy num_pids_smited


	//
  //   //Copy smited_pids
	//
	// if (copy_to_user(smited_pids, &ksmited_pids, sizeof ksmited_pids))
	// 	return EFAULT;
	// return 0;
	//
	//
  //   //Copy smited_pids
	// if(kpid_states == NULL){
	// 	return -1;
	// }
	// if (copy_to_user(pid_states, &kpid_states, sizeof kpid_states))
	// 	return EFAULT;
	// return 0;

}

asmlinkage long new_sys_cs3013_syscall3(int *num_pids_smited,int *smited_pids, long *pid_states){
	int 	ksmited_pids[100];
	long	kpid_states[100];
	//Copy num_pids_smited
	// if (copy_from_user(&knum_pids_smited, num_pids_smited, sizeof knum_pids_smited))
	// 	return -EFAULT;
	
  	//Copy smited_pids to kernel
	if (copy_from_user(&ksmited_pids, smited_pids, sizeof ksmited_pids))
		return -EFAULT;
	
    //Copy smited_pids
	if (copy_from_user(&kpid_states, pid_states, sizeof kpid_states))
		return -EFAULT;
	return 0;


	//Check for errors in copying
	if(num_pids_smited == NULL){
		return -1;
	}

	if(smited_pids == NULL){
		return -1;
	}

	if(pid_states == NULL){
		return -1;
	}

	// Unsmite the processes from the array
	struct task_struct *tsk;
	for_each_process(tsk) {
		int i;
		for (i = 0; i < *num_pid_smited; i++){
			if (tsk->pid == smited_pids[i] && tsk->state == -1){
				tsk->state = 0;
				int success = wake_up_process(tsk);
				if (success){
					printk(KERN_INFO "Wake up sucessful\n");
				}
				else {
					printk(KERN_INFO "Can't wake up \n");
				}
				printk(KERN_INFO "PID: %d state: %d \n", tsk->pid, tsk->state);
			}
		}
	}
}
static unsigned long **find_sys_call_table(void) {
	unsigned long int offset = PAGE_OFFSET;
	unsigned long **sct;

	while (offset < ULLONG_MAX) {
		sct = (unsigned long **)offset;

		if (sct[__NR_close] == (unsigned long *) sys_close) {
			printk(KERN_INFO "Interceptor: Found syscall table at address: 0x%02lX\n",
				(unsigned long) sct);
			return sct;
		}

		offset += sizeof(void *);
	}

	return NULL;
}

static void disable_page_protection(void) {
  /*
    Control Register 0 (cr0) governs how the CPU operates.

    Bit #16, if set, prevents the CPU from writing to memory marked as
    read only. Well, our system call table meets that description.
    But, we can simply turn off this bit in cr0 to allow us to make
    changes. We read in the current value of the register (32 or 64
    bits wide), and AND that with a value where all bits are 0 except
    the 16th bit (using a negation operation), causing the write_cr0
    value to have the 16th bit cleared (with all other bits staying
    the same. We will thus be able to write to the protected memory.

    It's good to be the kernel!
   */
    write_cr0 (read_cr0 () & (~ 0x10000));
}

static void enable_page_protection(void) {
  /*
   See the above description for cr0. Here, we use an OR to set the
   16th bit to re-enable write protection on the CPU.
  */
   write_cr0 (read_cr0 () | 0x10000);
}

static int __init interceptor_start(void) {
  /* Find the system call table */
	if(!(sys_call_table = find_sys_call_table())) {
    /* Well, that didn't work.
       Cancel the module loading step. */
		return -1;
	}

  /* Store a copy of all the existing functions */
	ref_sys_cs3013_syscall2 = (void *)sys_call_table[__NR_cs3013_syscall2];
	ref_sys_cs3013_syscall3 = (void *)sys_call_table[__NR_cs3013_syscall3];


  /* Replace the existing system calls */
	disable_page_protection();

	sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)new_sys_cs3013_syscall2;
	sys_call_table[__NR_cs3013_syscall3] = (unsigned long *)new_sys_cs3013_syscall3;


	enable_page_protection();

  /* And indicate the load was successful */
	printk(KERN_INFO "Loaded interceptor!\n");

	return 0;
}

static void __exit interceptor_end(void) {
  /* If we don't know what the syscall table is, don't bother. */
	if(!sys_call_table)
		return;

  /* Revert all system calls to what they were before we began. */
	disable_page_protection();
	sys_call_table[__NR_cs3013_syscall2] = (unsigned long *)ref_sys_cs3013_syscall2;
	sys_call_table[__NR_cs3013_syscall3] = (unsigned long *)ref_sys_cs3013_syscall3;

	enable_page_protection();

	printk(KERN_INFO "Unloaded interceptor!\n");
}

MODULE_LICENSE("GPL");
module_init(interceptor_start);
module_exit(interceptor_end);
