#include "arduinoFFT.h"
 
#define SAMPLES 128             //Must be a power of 2
#define SAMPLING_FREQUENCY 4000 //Hz, must be less than 10000 due to ADC
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];

void setup() {
  // put your setup code here, to run once:
  
//We want portD 1 for A0, but GPIO is default as analog so lucky us
//We want to use ADC0, ADC0_SE5b, AD5b
//Single ended channels AD23- AD4, we want AD5

Serial.begin(38400);
sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));

//ADC0 Configuration register 1
ADC0_CFG1 |= _BV(5) | (1U << 6); // clock division of 8. 9MHZ, 
ADC0_CFG1 |= (1U << 2) | (1U << 3); //16 bit conversion mode

//ADC0 configuration register 2
ADC0_CFG2 |= _BV(4); //This selects ADXXb, in our case AD05b or just AD5b
//ADC0_CFG2 |= ADC_CFG2_MUXSEL; //This does the same thing as above

//ADC0 Programmable gain amplifier
//ADC0_PGA |= ADC_PGA_PGAEN; //Use this line if you want to use programmable gain amplified that Professor discussed

//ADC0 Status and control register 3
ADC0_SC3 |= ADC_SC3_ADCO | ADC_SC3_AVGE; //These are in kinetis.h, enables continuous mode and averaging function. You don't NEED averaging function. 

//ADC0 Status control register 1
ADC0_SC1A = 4; // or binary 0101, it is equal not OR equal since I disabled the other configurations on purpose.
}

void loop() {
  // put your main code here, to run repeatedly:

/*SAMPLING*/

    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overflows after around 70 minutes!

        uint16_t data = ADC0_RA; // Data result register A
        float volt = ((float)data / 65535.0)*3.3; // conversion
     
        vReal[i] = volt;
        vImag[i] = 0;
     
        while(micros() < (microseconds + sampling_period_us)){
        }
    }
 
    /*FFT*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);
 
    /*PRINT RESULTS*/ 
    for(int i=0; i<(SAMPLES/2); i++)
    {
        /*View all these three lines in serial terminal to see which frequencies has which amplitudes*/         
        Serial.print((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
        Serial.print(" ");
        Serial.println(vReal[i], 1);    //View only this line in serial plotter to visualize the bins
    }

    Serial.println(peak);     //Print out what frequency is the most dominant.
    delay(100);
    
}
