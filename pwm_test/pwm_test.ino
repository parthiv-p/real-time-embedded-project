void setup() {
Serial.begin(38400);
delay(1000);

PORTD_PCR4 |= (1U << 10); //MUX,pin 6

////////////////////////////////////////////////////////////  PWM Section ////////////////////////////////////////////////////////////////////

//PTD4, chip 61, board 6 PWM  ...............FTM0 channel 4

FTM0_SC |= _BV(3); //Status control register , This selects system clock
FTM0_SC &= ~(_BV(4));

//DISABLE WRITE PROTECTION
FTM0_MODE |= (1U << 2); //Mode register

FTM0_MODE |= _BV(1); // Enables FTM, this must come after the line above since this bit is write protected.

FTM0_SC |= 0x4; //set bit 0 1, USING 16 BIT RESOLUTION

//Set MSB and ELSB, bit 5 and bit 3. This sets edge aligned PWM mode
FTM0_C4SC |= (1U << 3) | (1U << 5);

//Requirements for edge aligned up counter pwm is done, MSnB 1 (Channel mode select B) ,QUADEN 0 (Quadrature Decoder), DECAPEN 0 (Dual edge capture) , COMBINE 0 (Pair dual channel)
//CPWMS 0 (center aligned disabled and set as upcounting mode)

FTM0_CNTIN = 0; //Initial value is 0 for PWM counter, also edge align mode MUST use CNTIN by design
FTM0_MOD = 29999;//counts up to MOD then counts back up from 0, this is the period of cycle

}

void loop() {
FTM0_C4V = 1500;
delay(2000);
FTM0_C4V=2250;
delay(2000);
FTM0_C4V=3000;
delay(2000);


}
