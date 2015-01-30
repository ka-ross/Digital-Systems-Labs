**Documentation for Lab 2** 

The purpose of this lab assignment was to use the given framework of a shell, and to manipulate it so as to have a shell that could set/read the LEDs as well as utilize an egg timer.

**GLOBAL VARIABLES**

Variable Name | functionality
------------ | -------------
char *light_state[] | Used to read the state of a LED at any time
int init_time | this is set alongside *time* when timerset is run. it is used for resetting to the last chosen time. (-1 means not set)
int  time | this is the global reference to how much time is left on the egg timer. (-1 means not set)
int isRunning | a flag that stops/starts the timing threads

**Threads**
  
  * *CounterThread* - Thread that spins lights around in a circle while the timer is running. This only runs while isRunning is set to '1'.  The thread iterates a for loop, it initially checks whether or not the timer has been turned off. if so, it stops running, but if not, the lights continue to turn on in a circle, and the state for that light is appropriately adjusted. it sleeps for 125 milliseconds so that the North LED lights up every whole second.
  * *TimerThread* - Thread to count down when egg timer is running. The entire functionality of this thread revolves around whether or not isRunning is set to 1. The thread subtracts the time global variable by one each iteration and every time that timer mod 1000 equals 0, the console prints how many seconds are left in the timer. Then, when the time is zero, isRunning is set to 0 and the timer is set to -1. 
  

**Functions**

* cmd_timerSet(**int given_time**) - Sets time and init_time to the command-given time, as long as it is between 1-10000. This **DOES NOT** set the isRunning variable to 1.
* cmd_timerReset - Sets time back to init_time's value. this also clears all lights, and turns light state to "off"
* cmd_timerStart - Clears all lights, and then Sets isRunning to 1.
* cmd_timerStop - Sets isRunning to 0.
* cmd_timerGetTime - executes a chprintf of the time variable. can be run at any time.
* cmd_ledset(**char compass_direction**, **On/Off**) - Checks first if the value is 'N' or 'S', and if so, then checks if the next value is an 'E' or 'W', depending on these conditions sets the 'light' variable to the macro of the corresponding light. this works for each direction. the on or off is then assessed and then appropriately utilized to turn on or off the value. the light_state is appropriately changed.
* cmd_ledread(**char compass_direction**) - Checks first if the value is 'N' or 'S', and if so, then checks if the next value is an 'E' or 'W', depending on these conditions sets the 'light' variable to the macro of the corresponding light. this works for each direction. the light_state array is then checked for that macro's value - 8, and the state of that light is delivered.
