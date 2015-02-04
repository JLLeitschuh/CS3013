CS3013 Part 1
Team 48
Alexandra Bittle - albittle
Jonathan Leitschuh - jlleitschuh
Long Nguyen  - lhnguyen

How to use:
  To compile module run 'make'.
  To install module run 'sudo insmod antiVirusScanner.ko'
  You can test by running 'tail -f /var/log/syslog' and then opening the included test file
  You can test the functionaity of the virus printing by using 'tail -f /var/log/syslog | grep Virus'
    and then opening the test file.
  To remove the module run 'sudo rmmod antiVirusScanner.ko'
