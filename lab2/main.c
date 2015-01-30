/*finished on 1/27/15
 * rosskyle &
 * gbdierfe */

#include "ch.h"
#include "hal.h"
#include "test.h"
#include "shell.h" 
#include "chprintf.h"
#include <chstreams.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define UNUSED(x) (void)(x)
//Tells the light state for each light
char *light_state[] = {"off", "off","off","off","off","off","off","off"};
int init_time = -1;//for reset values, -1 means "not set"
int time = -1;//for countdown, -1 means "not set"
int isRunning = 0;//a flag used to communicate with threads whether or not the timer is running


static THD_WORKING_AREA(waShell,2048);

static thread_t *shelltp1;

/* Thread that blinks North LED as an "alive" indicator */
static THD_WORKING_AREA(waCounterThread,128);
static THD_FUNCTION(counterThread,arg) {
  UNUSED(arg);
  int i = 8;//used for looping 
  while (TRUE) {
    while(isRunning){
    for ( i=9; i<16; i++) {
      if(!isRunning){//if the timer stops running
	palSetPad(GPIOE, i-1);
	light_state[0] = "on";
	break;//gets out of the for loop
      }
      palSetPad(GPIOE, i);//dynamically sets current light on
      light_state[(i-8)] = "on";//turns state to "on"
      chThdSleepMilliseconds(125);
      palClearPad(GPIOE, i);//turns current off
      light_state[(i-8)] = "off";//light state is "off"
      if (i == 15){//restart looping
	i = 7;
      }
    }
    
    }
    chThdSleepMilliseconds(1);//escapes thread for scheduler
  }
  return 0;
}

static THD_WORKING_AREA(waTimerThread,128);
static THD_FUNCTION(timerThread,arg) {
  UNUSED(arg);
  while (TRUE) {
      int j;
    while(isRunning){
      if(time > 0){
	time--;//counting down
	if((time%1000)==0){
	  chprintf((BaseSequentialStream*)&SD1, "\n\r%d...\n\r", time/1000);//prints every second, and onscreen timer
	    }
      } else if (time == 0){
	time--;
	isRunning = 0;
	//all lights on
	palSetPad(GPIOE, GPIOE_LED4_BLUE);
	palSetPad(GPIOE, GPIOE_LED3_RED);
	palSetPad(GPIOE, GPIOE_LED5_ORANGE);
	palSetPad(GPIOE, GPIOE_LED7_GREEN);
	palSetPad(GPIOE, GPIOE_LED9_BLUE);
	palSetPad(GPIOE, GPIOE_LED10_RED);
	palSetPad(GPIOE, GPIOE_LED8_ORANGE);
	palSetPad(GPIOE, GPIOE_LED6_GREEN);
	chprintf((BaseSequentialStream*)&SD1, "\n\rch>");
	//sets state for all lights
	for(j=0; j<8; j++){
	light_state[j]="on";
	  }
      }
      //used so that the thread waits 1 ms for timer
      chThdSleepMilliseconds(1);
    }
    chThdSleepMilliseconds(1); //escapes for scheduler
  }
  return 0;
}



//sets the timer to a given value
static void cmd_timerSet(BaseSequentialStream *chp, int argc, char *argv[]){
  int i;
  if(argc == 1){
    i = atoi(argv[0]);
    if(i <= 0){
      init_time = 0;
      time = 0;
      isRunning = 0;
    }else if(i <= 10000){
      init_time = i;
      time = i;
      isRunning = 0;
    }else if(i > 10000){//if greater than 10,000, it sets to 10000
      init_time = 10000;
      time = 10000;
      isRunning = 0;
  }
    chprintf(chp, "Timer set to: %d\n\r", time);
}
}
//sets the timer to the init_time, and turns off all lights
static void cmd_timerReset(BaseSequentialStream *chp, int argc, char *argv[]){
  if(argc == 0) {
    int j;
    time = init_time;
    chprintf(chp, "Reset time to %d\n\r", init_time);
     //all lights off
      palClearPad(GPIOE, GPIOE_LED4_BLUE);
      palClearPad(GPIOE, GPIOE_LED3_RED); 
      palClearPad(GPIOE, GPIOE_LED5_ORANGE);
      palClearPad(GPIOE, GPIOE_LED7_GREEN);
      palClearPad(GPIOE, GPIOE_LED9_BLUE);
      palClearPad(GPIOE, GPIOE_LED10_RED);
      palClearPad(GPIOE, GPIOE_LED8_ORANGE);
      palClearPad(GPIOE, GPIOE_LED6_GREEN);
      for(j=0; j<8; j++){
	light_state[j]="off";
	  }
  }
}
//starts timer, obviously.
static void cmd_timerStart(BaseSequentialStream *chp, int argc, char *argv[]){
  if (argc == 0){
    int j;
    //all lights off
      palClearPad(GPIOE, GPIOE_LED4_BLUE);
      palClearPad(GPIOE, GPIOE_LED3_RED); 
      palClearPad(GPIOE, GPIOE_LED5_ORANGE);
      palClearPad(GPIOE, GPIOE_LED7_GREEN);
      palClearPad(GPIOE, GPIOE_LED9_BLUE);
      palClearPad(GPIOE, GPIOE_LED10_RED);
      palClearPad(GPIOE, GPIOE_LED8_ORANGE);
      palClearPad(GPIOE, GPIOE_LED6_GREEN);
      for(j=0; j<8; j++){
	light_state[j]="off";
      }
      isRunning = 1;
    chprintf(chp, "Starting timer at %d\n\r", time);
  }
}
//stops the timer
static void cmd_timerStop(BaseSequentialStream *chp, int argc, char *argv[]){
  if (argc == 0);
    isRunning = 0;
    chprintf(chp, "Timer stopped at %d...Collaborate and listen!\n\r", time);
}  


//gets time left on time variable
static void cmd_timerGetTime(BaseSequentialStream *chp, int argc, char *argv[]){
  if (argc == 0){
    chprintf(chp, "Remaining time: %d\n\r", time);
  }
}

//custom sets a led to ON or OFF
static void cmd_ledset(BaseSequentialStream *chp, int argc, char *argv[]){
char light;
 if (argc == 2){
   if(*argv[0] == 'N'){//"Is the first character an 'N'?"
     if(*(argv[0]+1) == 'E'){//checks if the second is an E
       light = GPIOE_LED5_ORANGE;
     } else if (*(argv[0]+1) == 'W'){//checks if second is a W
       light = GPIOE_LED4_BLUE;
     } else { light = GPIOE_LED3_RED;//else its simply N
     }
   } else if (*argv[0] == 'S') {//first is S
     if(*(argv[0]+1) == 'E'){//second is E
       light = GPIOE_LED9_BLUE;
     } else if (*(argv[0]+1) == 'W'){//second is W
       light = GPIOE_LED8_ORANGE;
     } else { 
       light = GPIOE_LED10_RED;//just S
     }
   } else if (*argv[0] == 'E'){ // Simply E
     light =  GPIOE_LED7_GREEN;
   } else { //first is West.
     light =  GPIOE_LED6_GREEN;
   }

//light state changes
   if (strcmp(argv[1], "on")==0){
       palSetPad(GPIOE, light); 
       light_state[(light-8)] = "on";
   } else if (strcmp(argv[1], "off") == 0){
     palClearPad(GPIOE, light); 
     light_state[(light-8)] = "off";
   }
 }
}

//delivers light state, using same method as led_set
static void cmd_ledread(BaseSequentialStream *chp, int argc, char *argv[]){
char light; 
 if (argc == 1) {
     if(*argv[0] == 'N'){
     if(*(argv[0]+1) == 'E'){
       light = GPIOE_LED5_ORANGE;
     } else if (*(argv[0]+1) == 'W'){
       light = GPIOE_LED4_BLUE;
     } else { light = GPIOE_LED3_RED;
     }
   } else if (*argv[0] == 'S') {
     if(*(argv[0]+1) == 'E'){
       light = GPIOE_LED9_BLUE;
     } else if (*(argv[0]+1) == 'W'){
       light = GPIOE_LED8_ORANGE;
     } else { 
       light = GPIOE_LED10_RED;
     }
   } else if (*argv[0] == 'E'){
     light =  GPIOE_LED7_GREEN;
   } else {
     light =  GPIOE_LED6_GREEN;
   }
     chprintf(chp, "Light state of %s is %s\n\r", argv[0], light_state[(light-8)]);

  }
}
  

static void cmd_myecho(BaseSequentialStream *chp, int argc, char *argv[]) {
  int32_t i;

  (void)argv;

  for (i=0;i<argc;i++) {
    chprintf(chp, "%s\n\r", argv[i]);
  }
}



static const ShellCommand commands[] = {
  {"myecho", cmd_myecho},
  {"ledset", cmd_ledset},
  {"ledread", cmd_ledread},
  {"timerset", cmd_timerSet},
  {"timerreset", cmd_timerReset},
  {"timerstart", cmd_timerStart},
  {"timerstop", cmd_timerStop},
  {"timergettime", cmd_timerGetTime},
  {NULL, NULL}//used for mistakes or non-specified values
};


static const ShellConfig shell_cfg1 = {
  (BaseSequentialStream *)&SD1,
  commands
};

static void termination_handler(eventid_t id) {

  (void)id;
  chprintf((BaseSequentialStream*)&SD1, "Shell Died\n\r");

  if (shelltp1 && chThdTerminatedX(shelltp1)) {
    chThdWait(shelltp1);
    chprintf((BaseSequentialStream*)&SD1, "Restarting from termination handler\n\r");
    chThdSleepMilliseconds(100);
    shelltp1 = shellCreate(&shell_cfg1, sizeof(waShell), NORMALPRIO);
  }
}

static evhandler_t fhandlers[] = {
  termination_handler
};

/*
 * Application entry point.
 */

int main(void) {
  event_listener_t tel;
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();

  /*
   * Activates the serial driver 1 using the driver default configuration.
   * PC4(RX) and PC5(TX). The default baud rate is 9600.
   */
  sdStart(&SD1, NULL);
  palSetPadMode(GPIOC, 4, PAL_MODE_ALTERNATE(7));
  palSetPadMode(GPIOC, 5, PAL_MODE_ALTERNATE(7));
  chprintf((BaseSequentialStream*)&SD1, "\n\rUp and Running\n\r");

  /* Initialize the command shell */ 
  shellInit();

  /* 
   *  setup to listen for the shell_terminated event. This setup will be stored in the tel  * event listner structure in item 0
   */
  chEvtRegister(&shell_terminated, &tel, 0);

  shelltp1 = shellCreate(&shell_cfg1, sizeof(waShell), NORMALPRIO);
  chThdCreateStatic(waCounterThread, sizeof(waCounterThread), NORMALPRIO+1, counterThread, NULL);  chThdCreateStatic(waTimerThread, sizeof(waTimerThread), NORMALPRIO+1, timerThread, NULL);

  while (TRUE) {
    chEvtDispatch(fhandlers, chEvtWaitOne(ALL_EVENTS));
  }
 }


