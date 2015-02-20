/*

  Title: Lab 4 - Elevator Altimeter Readings
  Completed: 2/03/15
  @authors: rosskyle & dianyu
 
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
#include <math.h>

#define UNUSED(x) (void)(x)

//GLOBAL VARIABLES
int isLogging = 0;//checks if the user button was pressed. defaults to 0.
int press_ft[3500];
//int press_m[3];
//int press_time[3];
int counter = 0;
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
static double get_pressure(){
  pressure_write_register(0x20, 0);//these three are done to initialize the "one-shot" reading.
  pressure_write_register(0x10, 0x7A);
  pressure_write_register(0x20, 0x84);
  pressure_write_register(0x21, 0x1);//gets a measurement
  uint32_t xl = pressure_read_register(0x28);//registers 40 -42 have the values for the pressure. 
  uint32_t l = pressure_read_register(0x29)<<8;
  uint32_t h = pressure_read_register(0x2A)<<16;
  uint32_t p = h | l | xl;//put them all together
  //chprintf((BaseSequentialStream*)&SD1, "Pressure_1: %d\n\r ", p/4096);//print for testing purposes
  double f = (double) ((double) p)/4096.0;//convert to millibars and then to a double.
  return f;  
}
/* Thread that blinks North LED as an "alive" indicator */
/* static THD_WORKING_AREA(waCounterThread,128); */
/* static THD_FUNCTION(counterThread,arg) { */
/*   UNUSED(arg); */
/*   while (TRUE) { */
/*     palSetPad(GPIOE, GPIOE_LED3_RED); */
/*     chThdSleepMilliseconds(500); */
/*     palClearPad(GPIOE, GPIOE_LED3_RED); */
/*     chThdSleepMilliseconds(500); */
/*   } */
/*   return 0; */
/* } */

static THD_WORKING_AREA(waBlinkerThread,128);
static THD_FUNCTION(blinkerThread,arg) {
  UNUSED(arg);
  while(TRUE){
  while (isLogging) {
    palSetPad(GPIOE, GPIOE_LED10_RED);
    chThdSleepMilliseconds(500);
    palClearPad(GPIOE, GPIOE_LED10_RED);
    chThdSleepMilliseconds(500);
  }
  chThdSleepMilliseconds(1);
  }
  return 0;
}


static THD_WORKING_AREA(waButtonThread,128);
static THD_FUNCTION(buttonThread,arg) {
  UNUSED(arg);
  while (TRUE) {
    switch(isLogging){
    case 0:
      if(palReadPad(GPIOA, GPIOA_BUTTON)){
	while (palReadPad(GPIOA, GPIOA_BUTTON)){
       	  isLogging = 0;
	  
       	}
	isLogging = 1;

      }
      break;
    case 1:
      if(palReadPad(GPIOA, GPIOA_BUTTON)){
	while (palReadPad(GPIOA, GPIOA_BUTTON)){
       	  isLogging = 1;
       	}
	isLogging = 0;
      }
      break;
    }
    chThdSleepMilliseconds(1); 
  }
  return 0;
}

static THD_WORKING_AREA(waLogThread,128);
static THD_FUNCTION(logThread,arg) {
  UNUSED(arg);
  while(TRUE){

    //  int i =0;
    while (isLogging && counter < 3500) {

      // for( i; i<(sizeof(press_ft)); i++){
      double press = get_pressure();
      chprintf((BaseSequentialStream*)&SD1, "1. %04d\n\r ", (int)press);//time stamp.

      //=(1-(A18/1013.25)^0.190284)*145366.45
      double altitude_ft = (1-pow((press/1013.25),0.190284))*145366.45;
      //converts to Altitude measurement

    
      //converts above value to int for printing purposes
      int final_ft = (int) altitude_ft;
	


      chprintf((BaseSequentialStream*)&SD1, "2.  %04d\n\r ", final_ft);//time stamp.

      press_ft[counter]= final_ft;
      // int final_m = (int)final_ft/3.280839895;
      //converts to meters, then to an int for printing purposes. 
      int final_m = (int) final_ft/3.280839895;

      chprintf((BaseSequentialStream*)&SD1, "3.  %04d\n\r ",final_m);//time stamp.
      chprintf((BaseSequentialStream*)&SD1, "array.  %04d\n\r ",press_ft[counter]);//aray value
      chprintf((BaseSequentialStream*)&SD1, "iterater.  %04d\n\r ",counter);//i.

      counter++;
      chThdSleepMilliseconds(1); //escape for scheduler.
     
     
 
    //   while (isLogging) {
    //   int i=0;
      //  int j=0;

      /*

      for(i;i<(sizeof(press_ft)); i++){
	while(isLogging){
	  double press = get_pressure();
	  chprintf((BaseSequentialStream*)&SD1, "1. %04d\n\r ", (int)press);//time stamp.
	  double altitude_ft = (1-pow((get_pressure()/1013.25),0.190284))*145366.45; //converts to Altitude measurement
	  int final_ft = (int) altitude_ft;//converts above value to int for printing purposes
	  // int final_m = (int)final_ft/3.280839895;//converts to meters, then to an int for printing purposes. 
	  chprintf((BaseSequentialStream*)&SD1, "2.  %04d\n\r ", (int)altitude_ft);//time stamp.
	  press_ft[i]= final_ft;
	  chprintf((BaseSequentialStream*)&SD1, "3.  %04d\n\r ", final_ft);//time stamp.
	  // press_m[i] = final_m;
	  //press_time[i] = j;
	  
	  //j++;
	}
      
      }

*/
      //  chThdSleepMilliseconds(1); //escape for scheduler.
    }
    chThdSleepMilliseconds(1); //escape for scheduler.
  }
  return 0;
}

static void cmd_dataPrint(BaseSequentialStream *chp, int argc, char *argv[]) {
  int rows = 0;
  int cols = 0;
  for(rows; rows<3500;rows++){
    chprintf((BaseSequentialStream*)&SD1, " %04d ", rows);//time stamp.
    chprintf((BaseSequentialStream*)&SD1, " %04d ", press_ft[rows]);//ft.
     int press_m = (int) press_ft[rows] / 3.280839895;
    chprintf((BaseSequentialStream*)&SD1, " %04d \n\r", press_m);//meters.
  }
}


static void cmd_press(BaseSequentialStream *chp, int argc, char *argv[]) {
  uint8_t address = (uint8_t) strtol(argv[1],NULL, 16);///used for hex conversion
  uint8_t value = (uint8_t) strtol(argv[2],NULL, 16);
 if(*argv[0] == 'r'){
   if(*argv[1] == 'a') {
     uint8_t i = 0;//these three used for table display of registers.
     uint8_t j = 0;
     uint8_t k = 0;
     chprintf((BaseSequentialStream*)&SD1, "Pressure: %d\n\r ", (int)get_pressure());
     chprintf((BaseSequentialStream*)&SD1, "         0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F  \n\r");
    
     for (i=0; i<4; i++){
       chprintf((BaseSequentialStream*)&SD1, "0x%02x0 |", i);//rows of table
       for(j=0; j<16; j++){
	 chprintf((BaseSequentialStream*)&SD1, " %03x ", pressure_read_register(k));//columns of table
        k++;
       }
       chprintf((BaseSequentialStream*)&SD1, "\n\r");
     }
   }
   chprintf((BaseSequentialStream*)&SD1, "Read Byte = 0x%02x\n\r",pressure_read_register(address));
   chprintf((BaseSequentialStream*)&SD1, "read byte = %d \n\r",pressure_read_register(address));
 } else if ( *argv[0] == 'w') {
   pressure_write_register(address, value);
   chprintf((BaseSequentialStream*)&SD1, "Wrote Byte = %02x at 0x%02x\n\r", value, address);
 }
}
static void cmd_gyro(BaseSequentialStream *chp, int argc, char *argv[]) {
  uint8_t address = (uint8_t) strtol(argv[1],NULL, 16);
  uint8_t value = (uint8_t)strtol(argv[2], NULL, 16);
 if(*argv[0] == 'r'){
   if(*argv[1] == 'a') {
     uint8_t i = 0;
     uint8_t j = 0;
     uint8_t k = 0;
     
     chprintf((BaseSequentialStream*)&SD1, "         0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F  \n\r");
    
     for (i=0; i<4; i++){
       chprintf((BaseSequentialStream*)&SD1, "0x%02x0 |", i);//rows of table
       for(j=0; j<16; j++){
	 chprintf((BaseSequentialStream*)&SD1, " %03x ", gyro_read_register(k));//columns
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
static void cmd_alti(BaseSequentialStream *chp, int argc, char *argv[]) {
 
  //=(1-(A18/1013.25)^0.190284)*145366.45
  double altitude_ft = (1-pow((get_pressure()/1013.25),0.190284))*145366.45; //converts to Altitude measurement
  int final_ft = (int) altitude_ft;//converts above value to int for printing purposes

  if(*argv[0] == 'f'){
    chprintf((BaseSequentialStream*)&SD1, "Altitude ft  = %d \n\r",final_ft);
  }
  else if (*argv[0] == 'm'){
    //=D18/3.280839895
    int altitude_m = (int)final_ft/3.280839895;//converts to meters, then to an int for printing purposes.
    chprintf((BaseSequentialStream*)&SD1, "Altitude meters  = %d \n\r",altitude_m);
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
  {"gyro", cmd_gyro},//added these three during this lab.
  {"press", cmd_press},
  {"altitude", cmd_alti},
  {"alt", cmd_alti}, 
  {"data", cmd_dataPrint},
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
  //chThdCreateStatic(waCounterThread, sizeof(waCounterThread), NORMALPRIO+1, counterThread, NULL);
  chThdCreateStatic(waButtonThread, sizeof(waButtonThread), NORMALPRIO+1, buttonThread, NULL);
  chThdCreateStatic(waLogThread, sizeof(waLogThread), NORMALPRIO+1, logThread, NULL);
  chThdCreateStatic(waBlinkerThread, sizeof(waBlinkerThread), NORMALPRIO+1, blinkerThread, NULL);
  while (TRUE) {
    chEvtDispatch(fhandlers, chEvtWaitOne(ALL_EVENTS));
  }
 }
