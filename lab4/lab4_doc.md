#LAB 4
### Authors Dian Yu (Dianyu) & Kyle Ross(Rosskyle)

###Graph of Lab Results
![Elevator Graph](http://imgur.com/HI943ao.png)
This is a depiction of the results we received during our lab. The three graphs to the left depict each elevator individually, and the middle is the three put up against one another. The numbers on the larger graph are re-scaled so that they reflect the same scale. The last graph on the right is to show that based on the fastest speed gathered, how far each elevator would travel within a second. As you can see, Ballantine Hall is the fastest by far amongst the three elevators we tested. Both Swain and Lindley are very similar in speeds but Lindley proved to be the slowest of the three. 


Mark: Add GLOBAL VARIABLES, ALTITUDE LOG, static void cmd_dataPrint in SHELL COMMANDS, and {“data”, cmd_dataPrint} in Shell Command Aliasing

### Global Variables
**isLogging**

The log flag for blog log in and log out. 1 for log on to memory the altitude data; 0 for logout to stop memorizing altitude data. 

**press_ft[3500]**

An array called press_ft has size of 3500, to store 3500 altitude measurement for 5 minutes.

### GYRO READ/WRITE
**uint8_t gyro_read_register (uint8_t address)**

reads the address from gyro in uinr8_t and returns the value at that address.


**void gyro_write_register (uint8_t address, uint8_t data)**

helps write value/data into an address in uint8_t.



### HELPER FUNCTIONS
**static double get_pressure()**
* Used to get the pressure of the atmosphere currently.
* when called, the function runs the necessary calls for “One-Shot” measurements.
* since the data is stored in only 8-bits at a time, the high(“h”), low(“l”), and extra-low(“xl”) values are appropriately shifted and combined for an accurate reading.


###PRESSURE READ/WRITE
**uint8_t pressure_read_register (uint8_t address) **
* It takes in an address and retrieves that value that is stored at the register of the given address, it then returns this address
* This function utilizes connection across the SPI.



**void pressure_write_register(uint8t address, uint8_t data)**
* The function takes in an address and a piece of data. the address specified is then overwritten with the data that has been given.
* This function utilizes connection across the SPI.


###ALTITUDE LOG 
**static THD_FUNCTION(blinkerThread,arg)**

The South LED blinks to indicate the log on of altitude log memory. The South LED blinks per second.

**static THD_FUNCTION(buttonThread,arg)**

This thread switches the flag of the log on. When isLogging is equal to 0, once the button is pressed and released, set isLogging flag to be 1. If the button was not released, remains the status of isLogging as 0. When isLogging is equal to 1, once the button is pressed and released, set isLogging flag to be 0. If the button was not released, remains the status of isLogging as 1. 

**static THD_FUNCTION(logThread,arg)**

The log thread prints out the value of pressure, current altitude values readed in feet and meters, value saved in press_ft array with current iterator, and current iterator to make sure the value put in the press_ft array is correct.
 

### SHELL COMMANDS
**static void cmd_press(BaseSequentialStream *chp, int argc, char *argv[]) **
* Function uses the input given through the shell input.
* the use of this command is to specify whether to use the pressure_read_register(input as ‘r’), or the pressure_write_register(input as ‘w’). the input is read and the function will read the value of a register, or write a value to a register accordingly. also outputs a print statement.
* if the input of  “r all” is given, the function will print out the status of all registers.
* accessed by shell command “press <address> <value>”



**static void cmd_gyro(BaseSequentialStream *chp, int argc, char *argv[])**

* cmd_gyro function works with the command shell input in the usb port screen. It first takes second input, hex address, converts it to be uint8_t address using strtol(str, NULL, 16), and then stores the new address as “uint8_t address”. But when second input is not an address and begins with ‘a’, avoid to use argv[1] as an address. cmd_gyro third input is the value needs to put in the address which typed before, but happens only in the write. The value input is also a hex number value, and needs to  convert to be uint8_t number.

* When cmd_gyro’s first input is ‘r’, which represents “read”, then check the second input. If second command input is equal to ‘a’, then it represents “read all”. When tester typed “gyor r all”, cmd_gryo returns a table with 4 rows and 16  columns, rows displayed from 0x00 ~ 0x30, while columns signals 0 ~ F. All the values in each address placed in the corresponding cells. If the second input is not equal to ‘a’, but an address, then it would read and print out the value set in the address that typed in the command shell with the function gyro_read_register(address).

* When cmd_gyro’s first input is ‘w’, which represents “write”, cmd_gryo will write the a new typed value into the address typed with gyro_write_register(address, value) function.



**static void cmd_alti(BaseSequentialStream *chp, int argc, char *argv[])**
* This function is used to convert the given pressure in millibars, to Altitude-ft(“f”) or Altitude-meters(”m”)
* Function can be called with the shell command syntax “altitude <f or m>”
* Alitude is calculated by taking the pressure given by get_pressure, and putting it through the following equation:
        (1-pow((get_pressure()/1013.25),0.190284))*145366.45;



**static void cmd_myecho(BaseSequentialStream *chp, int argc, char *argv[])**

* cmd_myecho function is the function with original pulled file. It’s a test and sample function to show how to read and print the input from command shell.

**static void cmd_dataPrint(BaseSequentialStream *chp, int argc, char *argv[])**

*cmd_dataPrint function is the function that prints out the 3500 data stored in the press_ft in 3 columns. First column is the time. Second column is the altitude memorized in feet unit, and the third column contains the altitude value in meter converted from feet unit. 


####Shell Command Aliasing
**static const ShellCommand commands[]**

* This function displays all the tolerated command lines in this main. 
*We added {“gyro”, cmd_gyro} to get gyro {r/w} {address} {value} and gyro r all; 
*{“press”, cmd_press} to get press {r/w} {address} {value} and press r all;
* {“altitude”, cmd_alti} to get the altitude {f/m}; and {“alt”, cmd_alti} for simpler version of altitude; 
*{“data”, cmd_dataPrint} to print all the altitude value memorized in the press_ft array.


### MAIN
**int main(void)**

* In the main function, it initializes HAL, Kernel and configures device drivers.
* Activates the serial driver 1 using the driver default configuration. PC4(RX) and PC5(TX).
* The default baud rate is 9600.
* Setup the pins for the spi link on the GPIOA. This link connects to the pressure sensor and the gyro: GPIOA 5 -- SCK, GPIOA 6 -- MISO, GPIOA 7 -- MOSI, GPIOA 8 -- pressure sensor chip select, and GPIOE 3 -- gyro chip select.
* After that, deassert the pressure sensor chip select (GPIOA 8) and gyro chip select (GPIOE 3). Initialize the command shell and setup to listen for the shell_terminated event, and finally run the while(TRUE) loop for whole file.
