/*
    ChibiOS - Copyright (C) 2006-2014 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

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
char *light_state[] = {"off", "off","off","off","off","off","off","off","off","off","off"};
int init_time = -1;
int time = -1;
int isRunning = 0;

static THD_WORKING_AREA(waShell,2048);

static thread_t *shelltp1;

/* Thread that blinks North LED as an "alive" indicator */
static THD_WORKING_AREA(waCounterThread,128);
static THD_FUNCTION(counterThread,arg) {
  UNUSED(arg);
  while (TRUE) {
    palSetPad(GPIOE, GPIOE_LED3_RED);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOE, GPIOE_LED3_RED);
    chThdSleepMilliseconds(500);
  }
  return 0;
}

static THD_WORKING_AREA(waTimerThread,128);
static THD_FUNCTION(timerThread,arg) {
  UNUSED(arg);
  while (TRUE) {
    while(isRunning){
      if(time > 0){
	time--;
      } else if (time == 0){
	time--;
	 palSetPad(GPIOE, GPIOE_LED4_BLUE);
	 palSetPad(GPIOE, GPIOE_LED3_RED);
	 palSetPad(GPIOE, GPIOE_LED5_ORANGE);
	 palSetPad(GPIOE, GPIOE_LED7_GREEN);
	 palSetPad(GPIOE, GPIOE_LED9_BLUE);
	 palSetPad(GPIOE, GPIOE_LED10_RED);
	 palSetPad(GPIOE, GPIOE_LED8_ORANGE);
	 palSetPad(GPIOE, GPIOE_LED6_GREEN);
      }
    }
    chThdSleepMilliseconds(1);
  }
  return 0;
}



 
static void cmd_timerSet(BaseSequentialStream *chp, int argc, char *argv[]){
  int i;
  if(argc == 1){
    i = atoi(argv[0]);
    if(i <= 0){
      init_time = 0;
      time = 0;
      isRunning = 0;
    }else if(i < 10000){
      init_time = i;
      time = i;
      isRunning = 1;
    }else if(i > 10000){
      init_time = 10000;
      time = 10000;
      isRunning = 1;
  }
     chprintf(chp, "%s", argv[0]);
    chprintf(chp, "%d", time);
}
}

static void cmd_timerReset(BaseSequentialStream *chp, int argc, char *argv[]){
  if(argc == 0) {
    time = init_time;
    chprintf(chp, "Reset time to %d", init_time);
  }
}

static void cmd_timerStart(BaseSequentialStream *chp, int argc, char *argv[]){
  if (argc == 0){
    //all lights off
      palClearPad(GPIOE, GPIOE_LED4_BLUE);
      palClearPad(GPIOE, GPIOE_LED3_RED); 
      palClearPad(GPIOE, GPIOE_LED5_ORANGE);
      palClearPad(GPIOE, GPIOE_LED7_GREEN);
      palClearPad(GPIOE, GPIOE_LED9_BLUE);
      palClearPad(GPIOE, GPIOE_LED10_RED);
      palClearPad(GPIOE, GPIOE_LED8_ORANGE);
      palClearPad(GPIOE, GPIOE_LED6_GREEN);
    isRunning = 1;
    chprintf(chp, "Starting timer at %d", time);
  }
}

static void cmd_timerStop(BaseSequentialStream *chp, int argc, char *argv[]){
  if (argc == 0){
    //all lights off
      palClearPad(GPIOE, GPIOE_LED4_BLUE);
      palClearPad(GPIOE, GPIOE_LED3_RED); //omitted for the flashing
      palClearPad(GPIOE, GPIOE_LED5_ORANGE);
      palClearPad(GPIOE, GPIOE_LED7_GREEN);
      palClearPad(GPIOE, GPIOE_LED9_BLUE);
      palClearPad(GPIOE, GPIOE_LED10_RED);
      palClearPad(GPIOE, GPIOE_LED8_ORANGE);
      palClearPad(GPIOE, GPIOE_LED6_GREEN);
    isRunning = 0;
    chprintf(chp, "Timer stopped at %d...Collaborate and listen!", time);
  }  
}

static void cmd_timerGetTime(BaseSequentialStream *chp, int argc, char *argv[]){
  if (argc == 0){
    chprintf(chp, "Remaining time: %d", time);
  }
}

static void cmd_ledset(BaseSequentialStream *chp, int argc, char *argv[]){
  if (argc == 2){
    if (strcmp(argv[1],"on")==0){
      if (strcmp(argv[0],"N")==0){
	palSetPad(GPIOE, GPIOE_LED3_RED);
	light_state[3] = "on";
      }
      if (strcmp(argv[0],"NE")==0){
	palSetPad(GPIOE, GPIOE_LED5_ORANGE);
	light_state[5] = "on";;
      }
      if (strcmp(argv[0],"E")==0){
	palSetPad(GPIOE, GPIOE_LED7_GREEN);
	light_state[7] = "on";;
      }
      if (strcmp(argv[0],"SE")==0){
	palSetPad(GPIOE, GPIOE_LED9_BLUE);
	light_state[9] = "on";;
      }
      if (strcmp(argv[0],"S")==0){
	palSetPad(GPIOE, GPIOE_LED10_RED);
	light_state[10] = "on";;
      }
      if (strcmp(argv[0],"SW")==0){
	palSetPad(GPIOE, GPIOE_LED8_ORANGE);
	light_state[8] = "on";;
      }
       if (strcmp(argv[0],"W")==0){
	palSetPad(GPIOE, GPIOE_LED6_GREEN);
	light_state[6] = "on";;
      }
        if (strcmp(argv[0],"NW")==0){
	palSetPad(GPIOE, GPIOE_LED4_BLUE);
	light_state[4] = "on";;
      }
    } else if (strcmp(argv[1],"off") == 0) {
      if (strcmp(argv[0],"N")==0){
	palClearPad(GPIOE, GPIOE_LED3_RED);
	light_state[3] = "off";
      }
      if (strcmp(argv[0],"NE")==0){
	palClearPad(GPIOE, GPIOE_LED5_ORANGE);
	light_state[5] = "off";;
      }
      if (strcmp(argv[0],"E")==0){
	palClearPad(GPIOE, GPIOE_LED7_GREEN);
	light_state[7] = "off";;
      }
      if (strcmp(argv[0],"SE")==0){
	palClearPad(GPIOE, GPIOE_LED9_BLUE);
	light_state[9] = "off";;
      }
      if (strcmp(argv[0],"S")==0){
	palClearPad(GPIOE, GPIOE_LED10_RED);
	light_state[10] = "off";;
      }
      if (strcmp(argv[0],"SW")==0){
	palClearPad(GPIOE, GPIOE_LED8_ORANGE);
	light_state[8] = "off";;
      }
       if (strcmp(argv[0],"W")==0){
	palClearPad(GPIOE, GPIOE_LED6_GREEN);
	light_state[6] = "off";;
      }
        if (strcmp(argv[0],"NW")==0){
	palClearPad(GPIOE, GPIOE_LED4_BLUE);
	light_state[4] = "off";;
      }
    }
  }
}

static void cmd_ledread(BaseSequentialStream *chp, int argc, char *argv[]){
  if (argc == 1) {
    if (strcmp(argv[0], "N") ==0) {
	chprintf(chp, "State: %s", light_state[3]);
    }
    if (strcmp(argv[0], "NE") ==0) {
	chprintf(chp, "State: %s", light_state[5]);
    }
    if (strcmp(argv[0], "E") ==0) {
	chprintf(chp, "State: %s", light_state[7]);
    }
    if (strcmp(argv[0], "SE") ==0) {
	chprintf(chp, "State: %s", light_state[9]);
    }
    if (strcmp(argv[0], "S") ==0) {
	chprintf(chp, "State: %s", light_state[10]);
    }
    if (strcmp(argv[0], "SW") ==0) {
	chprintf(chp, "State: %s", light_state[8]);
    }
    if (strcmp(argv[0], "W") ==0) {
	chprintf(chp, "State: %s", light_state[6]);
    }
    if (strcmp(argv[0], "NW") ==0) {
	chprintf(chp, "State: %s", light_state[4]);
    }

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
  {"timergettime", cmd_timerGetTime}
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
  // chThdCreateStatic(waCounterThread, sizeof(waCounterThread), NORMALPRIO+1, counterThread, NULL); 
  chThdCreateStatic(waTimerThread, sizeof(waTimerThread), NORMALPRIO+1, timerThread, NULL);

  while (TRUE) {
    chEvtDispatch(fhandlers, chEvtWaitOne(ALL_EVENTS));
  }
 }


