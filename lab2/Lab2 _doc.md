Documentation for Lab 2 

The purpose of this lab assignment was to use the given framework of a shell, and to manipulate it so as to have a shell that could set/read the LEDs as well as utilize an egg timer.

*GLOBAL VARIABLES*

Variable Name | functionality
------------ | -------------
char *light_state[] | Used to read the state of a LED at any time
int init_time | this is set alongside **time** when timerset is run. it is used for resetting to the last chosen time. (-1 means not set)
int  time | this is the global reference to how much time is left on the egg timer. (-1 means not set)
int isRunning | a flag that stops/starts the timing threads



