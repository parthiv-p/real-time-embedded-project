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

#include "arduinoFFT.h"

uint8_t trig_pin = 22;
uint8_t echo_pin = 23;
uint8_t tone_pin = A0; // analog input 0 (pin 14)
uint8_t button_pin = 15;
uint8_t led_pin = 13; // to signal that we are finished

// we can change the order of the beacons by changing this list
double tone_list[] = {
  5000,
  5500,
  6000,
  6500,
  7000,
  7500,
  8000,
  8500,
  9000,
  9500
};

void setup(){
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(tone_pin, INPUT);
  pinMode(button_pin, INPUT_PULLDOWN);
  pinMode(led_pin, OUTPUT);
  Serial.begin(9600);
}
