/*
- splitting Arduino files is strange
- the preprocessor places this tab first, then concatenates the rest of the 
  tabs in alphabetical order into one single file
- because of this, we need to come up with a naming convention so that the code will compile

~~~ embedded_setup.ino ~~~
- contains all I/O pins, as well as their pinmodes

~~~ a_fsm ~~~
- to add a new state, we need to:
  1.) add the state to the state_t typedef
  2.) add any relevant keys to the events_t typedef
  3.) add all transitions to and from the new state in transition_table
  4.) update "z_flow" switch statement

~~~ b_*INPUT OR OUTPUT* ~~~
- for each sensor or controller that we will be using, we need a tab for the associated functions

~~~ c_*STATES IN FSM* ~~~
- here we will create a function for each state
- this function will likely use code that exists in the a_* and b_* tabs
- each state function should configure some input or output, then enter an infinite loop
  - this infinite loop needs at least one stop condition
  - each stop condition will then assign the global variable "key" to the event
  that just occured, which is used in the FSM flow
  - finally, a "break" is required

~~~ z_flow ~~~
- this is basically our "main"
- each state requires it's own case in the switch statement

*/

#include "MPU9250.h"

uint8_t trig_pin = 22;
uint8_t echo_pin = 23;

// Motor control global variables
float magBias[3] = {17.569, 20.618, -20.612}, magScale[3] = {0.992, 1.069, 0.946}; // Location specific
const float pi = 3.14159;

MPU9250 IMU(Wire,0x68);

void setup(){
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);

  // PWM channel setup
  PORTD_PCR4 |= (1U << 10); // mux to FTM0_C4 for pin 6 - left
  PORTD_PCR5 |= (1U << 10); // mux to FTM0_C5 for pin 20 - right
  FTM0_SC |= _BV(3); //Status control register , This selects system clock
  FTM0_SC &= ~(_BV(4));
  FTM0_MODE |= (1U << 2); //Mode register
  FTM0_MODE |= _BV(1); // Enables FTM, this must come after the line above since this bit is write protected.
  FTM0_SC |= 0x4; //set bit 0 1, USING 16 BIT RESOLUTION
  FTM0_C4SC |= (1U << 3) | (1U << 5); // channel 4
  FTM0_C5SC |= (1U << 3) | (1U << 5); // channel 5
  FTM0_CNTIN = 0; //Initial value is 0 for PWM counter, also edge align mode MUST use CNTIN by design
  FTM0_MOD = 29999;//counts up to MOD then counts back up from 0, this is the period of cycle
  
  Serial.begin(115200);

  // IMU Configuration
  int status = IMU.begin();
  if (status < 0) {
    Serial.println("IMU initialization unsuccessful");
    Serial.println("Check IMU wiring or try cycling power");
    Serial.print("Status: ");
    Serial.println(status);
    while(1) {}
  }

  // Magnetometer calibration
  IMU.setMagCalX(magBias[0], magScale[0]);
  IMU.setMagCalY(magBias[1], magScale[1]);
  IMU.setMagCalZ(magBias[2], magScale[2]);
  
  delay(100);

  float mxb, myb, mzb, mxs, mys, mzs;
  
  // Sanity check
  mxb = IMU.getMagBiasX_uT();
  myb = IMU.getMagBiasY_uT();
  mzb = IMU.getMagBiasZ_uT();

  mxs = IMU.getMagScaleFactorX();
  mys = IMU.getMagScaleFactorY();
  mzs = IMU.getMagScaleFactorZ();  

  Serial.println("Magnetometer Calibration Sanity Check:");
  Serial.println("mxb:\t" + String(mxb) + "\t mxs:\t" + String(mxs) + "\t myb:\t" + String(myb) + "\t mys:\t" + String(mys) + "\t mzb:\t" + String(mzb) + "\t mzs:\t" + String(mzs) );

  determineAngleShift(); // Reset starting reference frame
  
}
