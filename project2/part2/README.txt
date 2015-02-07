CS3013 Project 2 Part 2 
Team 48

Alexandra Bittle - albittle
Jonathan Leitschuh - jlleitchuh
Long Nguyen - lhnguyen

How to use:  
	To compile module run 'make' 
	To install module run 'sudo insmod moduleSmiteUnsmite.ko'   
	To test smite, run './smite [target id] > smiteOut.txt' 
	This will pipe the output into smiteOut.txt to be read by unsmite.
	Make sure that smite did not throw an error into smiteOut.txt
	To test unsmite, run './unsmite < smiteOut.txt' 
	To remove the module run 'sudo rmmod moduleSmiteUnsmite.ko'

The file OurTestResults.txt shows the syslog output from using smite and then unsmite on a user, and then reporting how much
time the user's process took to execute.
