/*
  Created by: Kyle Ross and Max Hollingsworth
  Date: Wed January 21, 2015
  Lab1 Egg Timer
*/
#include "ch.h"
#include "hal.h"
#include "test.h"
#include "chprintf.h"
#include <chstreams.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#define UNUSED(x) (void)(x)
int32_t global = -1; //-1 means timer not set, global holds the remaining time in milliseconds
//flashingThread flashes the north LED at 10hz when timer is on (i.e. global > 0)
static THD_WORKING_AREA(waFlashingThread, 128);
static THD_FUNCTION(flashingThread, arg) {
  UNUSED(arg);
  while(true) {
    if (global > 0) {
      palSetPad(GPIOE, GPIOE_LED3_RED);
      chThdSleepMilliseconds(50);
      palClearPad(GPIOE, GPIOE_LED3_RED);
      chThdSleepMilliseconds(50);
    }
    chThdSleepMilliseconds(1);//lets scheduler move on to another thread.
  }
  return 0;
}
/* Thread for timer:
handles countdown and button clear */
static THD_WORKING_AREA(waTimerThread, 128);
static THD_FUNCTION(timerThread, arg) {
  UNUSED(arg);
  uint8_t lightsON = 0; //flag for the end of the timer. starts false
  while(true) {
    //timer countdown is here
    if(global > 0) {
      global--;
    } else if (global == 0) { //timer goes off
      lightsON = 1; //flag on
      global--;
    } 
    //for the user button clear.
    //if pressed light flag is turned off and timer is reset
    if (palReadPad(GPIOA, GPIOA_BUTTON)) { 
      lightsON = 0;
      global = -1;
      palClearPad(GPIOE, GPIOE_LED3_RED);//added because of the omission below.
    }
    if (lightsON) {
      //all lights on
      palSetPad(GPIOE, GPIOE_LED4_BLUE);
      palSetPad(GPIOE, GPIOE_LED3_RED);
      palSetPad(GPIOE, GPIOE_LED5_ORANGE);
      palSetPad(GPIOE, GPIOE_LED7_GREEN);
      palSetPad(GPIOE, GPIOE_LED9_BLUE);
      palSetPad(GPIOE, GPIOE_LED10_RED);
      palSetPad(GPIOE, GPIOE_LED8_ORANGE);
      palSetPad(GPIOE, GPIOE_LED6_GREEN);
    } else {
      //all lights off
      palClearPad(GPIOE, GPIOE_LED4_BLUE);
      //palClearPad(GPIOE, GPIOE_LED3_RED); //omitted for the flashing
      palClearPad(GPIOE, GPIOE_LED5_ORANGE);
      palClearPad(GPIOE, GPIOE_LED7_GREEN);
      palClearPad(GPIOE, GPIOE_LED9_BLUE);
      palClearPad(GPIOE, GPIOE_LED10_RED);
      palClearPad(GPIOE, GPIOE_LED8_ORANGE);
      palClearPad(GPIOE, GPIOE_LED6_GREEN);
    }
    chThdSleepMilliseconds(1);//lets scheduler move on to another thread.
  }
  return 0;
}
/* Thread for user input */
//there is no sleep in this thread but the chnRead's are blocking so the 
// scheduler can move to other tasks.
static THD_WORKING_AREA(waSThread, 128);
static THD_FUNCTION(sThread, arg) {
  UNUSED(arg);
  uint8_t ch; //because input comes in character by character
  uint8_t buffer[6]; //timer as a string
  uint8_t place = 0;
  while(true){
    chnRead((BaseSequentialStream*)&SD1,&ch,1);
    if(ch == 115){ //if 's'
      chprintf((BaseSequentialStream*)&SD1, "\n\rEnter a value between 1 and 10000. \n\r");
      place = 0;
      while(true) {
    //fill buffer with input
    chnRead((BaseSequentialStream*)&SD1,&ch,1);
    if(place<5 && ch <= 57 && ch >= 48) { //length is less then 5 and must be an integer
      chnWrite((BaseSequentialStream*)&SD1,&ch,1);
      buffer[place] = ch;
      place++;
    }
    if (ch == 10) { //enter is pressed
      break;
    }
      }
      buffer[place] = '\0';
      global = atoi(buffer);
     
      if (global>10000) { //range is 1 - 10000
    global = 10000;
      }
      chprintf((BaseSequentialStream*)&SD1, "\n\r%d Selected. Counting Down... \n\r", global);
    }  
  }
  return 0;
}
/*
 * Application entry point.
 */
int main(void) {
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
  chprintf((BaseSequentialStream*)&SD1, "\n\rWelcome to Eggtimer.\n Press 's' to Enter a Value! \n\r");
  /*
   * Creates the threads.
   */
  chThdCreateStatic(waSThread, sizeof(waSThread), NORMALPRIO+1, sThread, NULL);
  chThdCreateStatic(waTimerThread, sizeof(waTimerThread), NORMALPRIO+1, timerThread, NULL);
  chThdCreateStatic(waFlashingThread, sizeof(waFlashingThread), NORMALPRIO+1, flashingThread, NULL);
  
  /*
    Main spins here while the threads do all of the work. 
  */ 
  while (TRUE);
}
