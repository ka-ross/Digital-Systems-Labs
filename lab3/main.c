/*

  rosskyle & dianyu

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

#define UNUSED(x) (void)(x)
static THD_WORKING_AREA(waShell,2048);

static thread_t *shelltp1;

/* SPI configuration, sets up PortA Bit 8 as the chip select for the pressure sensor */
static SPIConfig pressure_cfg = {
  NULL,
  GPIOA,
  8,
  SPI_CR1_BR_2 | SPI_CR1_BR_1,
  0
};

/* SPI configuration, sets up PortE Bit 3 as the chip select for the gyro */
static SPIConfig gyro_cfg = {
  NULL,
  GPIOE,
  3,
  SPI_CR1_BR_2 | SPI_CR1_BR_1,
  0
};

uint8_t pressure_read_register (uint8_t address) {
  uint8_t receive_data;

  address = address | 0x80;            /* Set the read bit (bit 7)         */
  spiAcquireBus(&SPID1);               /* Acquire ownership of the bus.    */
  spiStart(&SPID1, &pressure_cfg);     /* Setup transfer parameters.       */
  spiSelect(&SPID1);                   /* Slave Select assertion.          */
  spiSend(&SPID1, 1, &address);        /* Send the address byte */
  spiReceive(&SPID1, 1,&receive_data); 
  spiUnselect(&SPID1);                 /* Slave Select de-assertion.       */
  spiReleaseBus(&SPID1);               /* Ownership release.               */
  return (receive_data);
}

void pressure_write_register (uint8_t address, uint8_t data) {
  address = address & (~0x80);         /* Clear the write bit (bit 7)      */
  spiAcquireBus(&SPID1);               /* Acquire ownership of the bus.    */
  spiStart(&SPID1, &pressure_cfg);     /* Setup transfer parameters.       */
  spiSelect(&SPID1);                   /* Slave Select assertion.          */
  spiSend(&SPID1, 1, &address);        /* Send the address byte */
  spiSend(&SPID1, 1, &data); 
  spiUnselect(&SPID1);                 /* Slave Select de-assertion.       */
  spiReleaseBus(&SPID1);               /* Ownership release.               */
}

uint8_t gyro_read_register (uint8_t address) {
  uint8_t receive_data;

  address = address | 0x80;            /* Set the read bit (bit 7)         */
  spiAcquireBus(&SPID1);               /* Acquire ownership of the bus.    */
  spiStart(&SPID1, &gyro_cfg);         /* Setup transfer parameters.       */
  spiSelect(&SPID1);                   /* Slave Select assertion.          */
  spiSend(&SPID1, 1, &address);        /* Send the address byte */
  spiReceive(&SPID1, 1,&receive_data); 
  spiUnselect(&SPID1);                 /* Slave Select de-assertion.       */
  spiReleaseBus(&SPID1);               /* Ownership release.               */
  return (receive_data);
}

void gyro_write_register (uint8_t address, uint8_t data) {
  address = address & (~0x80);         /* Clear the write bit (bit 7)      */
  spiAcquireBus(&SPID1);               /* Acquire ownership of the bus.    */
  spiStart(&SPID1, &gyro_cfg);         /* Setup transfer parameters.       */
  spiSelect(&SPID1);                   /* Slave Select assertion.          */
  spiSend(&SPID1, 1, &address);        /* Send the address byte            */
  spiSend(&SPID1, 1, &data); 
  spiUnselect(&SPID1);                 /* Slave Select de-assertion.       */
  spiReleaseBus(&SPID1);               /* Ownership release.               */
}

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
static void cmd_press(BaseSequentialStream *chp, int argc, char *argv[]) {
  uint8_t address = (uint8_t) strtol(argv[1],NULL, 16);
  uint8_t value = (uint8_t)atoi(argv[2]);

 if(*argv[0] == 'r'){
   if(*argv[1] == 'a') {
     uint8_t i = 0;//First control register
     uint8_t j = 0;
     uint8_t k = 0;
     
     chprintf((BaseSequentialStream*)&SD1, "         0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F  \n\r");
    
     for (i=0; i<5; i++){
        chprintf((BaseSequentialStream*)&SD1, "0x%02x0 |", i);
       for(j=0; j<16; j++){
	chprintf((BaseSequentialStream*)&SD1, " %03i ", pressure_read_register(k));
	k++;
       //chprintf((BaseSequentialStream*)&SD1, "    %02x   |  0x%02x\n\r", i, gyro_read_register(i));
       }
       chprintf((BaseSequentialStream*)&SD1, "\n\r");
     }
   }
   chprintf((BaseSequentialStream*)&SD1, "Read Byte = 0x%02x\n\r",pressure_read_register(address));
   chprintf((BaseSequentialStream*)&SD1, "read byte = %d \n\r",pressure_read_register(address));
 } else if ( *argv[0] == 'w') {
   pressure_write_register(address, value);
   chprintf((BaseSequentialStream*)&SD1, "Wrote Byte = %d at 0x%02x\n\r", value, address);
 }

}

static void cmd_gyro(BaseSequentialStream *chp, int argc, char *argv[]) {
  uint8_t address = (uint8_t) strtol(argv[1],NULL, 16);
  uint8_t value = (uint8_t)atoi(argv[2]);

 if(*argv[0] == 'r'){
   if(*argv[1] == 'a') {
     uint8_t i = 0;//First control register
     uint8_t j = 0;
     uint8_t k = 0;
     
     chprintf((BaseSequentialStream*)&SD1, "         0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F  \n\r");
    
     for (i=0; i<4; i++){
        chprintf((BaseSequentialStream*)&SD1, "0x%02x0 |", i);
       for(j=0; j<16; j++){
	chprintf((BaseSequentialStream*)&SD1, " %03i ", gyro_read_register(k));
	k++;
       //chprintf((BaseSequentialStream*)&SD1, "    %02x   |  0x%02x\n\r", i, gyro_read_register(i));
       }
       chprintf((BaseSequentialStream*)&SD1, "\n\r");
     }
   }
   chprintf((BaseSequentialStream*)&SD1, "Read Byte = 0x%02x\n\r",gyro_read_register(address));
   chprintf((BaseSequentialStream*)&SD1, "read byte = %d \n\r",gyro_read_register(address));
 } else if ( *argv[0] == 'w') {
   gyro_write_register(address, value);
   chprintf((BaseSequentialStream*)&SD1, "Wrote Byte = %d at 0x%02x\n\r", value, address);
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
  {"gyro", cmd_gyro},
  {"press", cmd_press},
  {NULL, NULL}
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

  /* 
   *  Setup the pins for the spi link on the GPIOA. This link connects to the pressure sensor and the gyro.  
   * 
   */

  palSetPadMode(GPIOA, 5, PAL_MODE_ALTERNATE(5));     /* SCK. */
  palSetPadMode(GPIOA, 6, PAL_MODE_ALTERNATE(5));     /* MISO.*/
  palSetPadMode(GPIOA, 7, PAL_MODE_ALTERNATE(5));     /* MOSI.*/
  palSetPadMode(GPIOA, 8, PAL_MODE_OUTPUT_PUSHPULL);  /* pressure sensor chip select */
  palSetPadMode(GPIOE, 3, PAL_MODE_OUTPUT_PUSHPULL);  /* gyro chip select */
  palSetPad(GPIOA, 8);                                /* Deassert the pressure sensor chip select */
  palSetPad(GPIOE, 3);                                /* Deassert the gyro chip select */

  chprintf((BaseSequentialStream*)&SD1, "\n\rUp and Running\n\r");
  chprintf((BaseSequentialStream*)&SD1, "Gyro Whoami Byte = 0x%02x\n\r",gyro_read_register(0x0F));

  /* Initialize the command shell */ 
  shellInit();

  /* 
   *  setup to listen for the shell_terminated event. This setup will be stored in the tel  * event listner structure in item 0
  */
  chEvtRegister(&shell_terminated, &tel, 0);

  shelltp1 = shellCreate(&shell_cfg1, sizeof(waShell), NORMALPRIO);
  chThdCreateStatic(waCounterThread, sizeof(waCounterThread), NORMALPRIO+1, counterThread, NULL);

  while (TRUE) {
    chEvtDispatch(fhandlers, chEvtWaitOne(ALL_EVENTS));
  }
 }


