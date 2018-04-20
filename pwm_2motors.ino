/*
 *  20ms period
 *  pulse > 1.52ms: clockwise         ---> 2ms = 750 in C4V
 *  pulse < 1.48ms: counter clockwise ---> 1ms = 375 in C4V
 *  1.48ms < pulse < 1.52ms: stop
 * 
 */

void setup() {
Serial.begin(38400);
delay(1000);

PORTD_PCR4 |= (1U << 10); // mux to FTM0_C4 for pin 6
PORTD_PCR5 |= (1U << 10); // mux to FTM0_C5 for pin 20

/*
 * there are 3 FlexTimer modules.. FTM0, FTM1, FMT2... supports input capture, output compare, PWM
 *    - FTMx runs off of the BUS CLOCK
 *    - FTM time reference is a 16-bit counter... can be free running, or have stop and start, up or down
 *    - FTM0 has 8 channels. FTM0 has different usages than FTM1 and FTM2 (2 channels)
 *    - FTMs have multiple sources of interrupt, but they generate a single request...
 *        when an FTM interrupt occurs, read the FTM status register to determine the source
 *    - offers a prescaler divide-by 1, 2, 4, 8, 16, 32, 64, 128
 *
 */ 

// use system clock... this sets all channels in FTM0
FTM0_SC |= _BV(3); 
FTM0_SC &= ~(_BV(4));


// disable write protection
FTM0_MODE |= (1U << 2); //Mode register
FTM0_MODE |= _BV(1); // Enables FTM, this must come after the line above since this bit is write protected.

FTM0_SC |= 0x7; // prescale by factor of 128

// this register is used to configure the interrupt enable, channel config, and pin function
// set MSB and ELSB... sets edge aligned PWM mode, and high-true pulses (clear output on match)
FTM0_C4SC |= (1U << 3) | (1U << 5); // channel 4
FTM0_C5SC |= (1U << 3) | (1U << 5); // channel 5


FTM0_CNTIN = 0; // gives an initial value of 0 for the counter (used after reset)
FTM0_C5V = 562; //900; // count up to C5V and clear output, C5V/MOD duty cycle %
FTM0_C4V = 562; //900; // count up to C4V and clear output, C4V/MOD duty cycle %
FTM0_MOD = 7500; //7500; //5625;// counts up to MOD... this is the period of cycle

}

void loop() {

}
