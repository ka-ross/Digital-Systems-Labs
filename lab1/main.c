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

   
    
int main(void) {
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  //while (1);

  halInit();
  chSysInit();
  palSetPad(GPIOE, GPIOE_LED10_RED);
  /*
   * Activates the serial driver 1 using the driver default configuration.
   * PC4(RX) and PC5(TX). The default baud rate is 9600.
   */
  //    sdStart(&SD1, NULL);
  // palSetPadMode(GPIOC, 4, PAL_MODE_ALTERNATE(7));
  // palSetPadMode(GPIOC, 5, PAL_MODE_ALTERNATE(7));
  // chprintf((BaseSequentialStream*)&SD1, "\n\rWelcome to Eggtimer.\n Press 's' to Enter a Value! \n\r");
  
  //palSetPad(GPIOE, GPIOE_LED10_RED);
  
  /*
    Main spins here while the threads do all of the work. 
  */ 
  while (TRUE);
}
