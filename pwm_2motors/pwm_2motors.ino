
void initPWM(){
	FTM0_MODE |= (1U << 2) | (1U << 0); //Disable write protection
	FTM0_SC |= (1U << 3) | (1U << 2);//use system clock, 16 prescalar

	FTM0_C0SC |= (1U << 3) | (1U << 5); //edge aligned high output pc1
	FTM0_C1SC |= (1U << 3) | (1U << 5); //edge aligned high output pc2

	FTMO_CNTIN = 0;
	FTM0_MOD = 29999; //sets to 50hz period

	PORTC_PCR1 |= (1U << 10); //22 FTM0 ch0 alt 4
	PORTC_PCR2 |= (1U << 10); //23 FTM0 ch1 alt 4
}

void forward(){
	FTM0_C0V = 1500;
	FTM0_C1V = 1500;
}

void reverse(){
	FTM0_C0V = 3000;
	FTM0_C1V = 3000;
}

void left(){
	FTM0_C0V = 1500;
	FTM0_C1V = 3000;
}

void right(){
	FTM0_C0V = 3000;
	FTM0_C1V = 1500;
}

void stop(){
	FTM0_C0V = 2250;
	FTM0_C1V = 2250;
}

void setup(){
	initPWM();	

}

void loop(){
	forward();
	delay(2000);
	stop();
	delay(2000);
	reverse();
	delay(2000);
	left();
	delay(2000);
	right();
	delay(2000);
}