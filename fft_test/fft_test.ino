#include "arduinoFFT.h"
 
#define SAMPLES 128           //Must be a power of 2
#define SAMPLING_FREQUENCY 22000 //Hz, must be less than 10000 due to ADC
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];

unsigned long Now;

void setup() {
  // put your setup code here, to run once:
  
//We want portD 1 for A0, but GPIO is default as analog so lucky us
//We want to use ADC0, ADC0_SE5b, AD5b
//Single ended channels AD23- AD4, we want AD5

Serial.begin(38400);
pinMode(16, INPUT);
sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
}

void loop() {
  // put your main code here, to run repeatedly:

/*SAMPLING*/
    Now = micros();
    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overflows after around 70 minutes!

        uint16_t data = analogRead(16); // Data result register A        
        vReal[i] = (double)data;
        vImag[i] = 0;
     
        while(micros() < (microseconds + sampling_period_us)){
        }
    }
 
    /*FFT*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

    //Serial.println("Time: " + String(micros() - Now));
    
    double peak = FFT.MajorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);

    float amp = vReal[55];
    if(amp < 10.0f)
      amp = 0;
 
    /*PRINT RESULTS*/ 
    /*
    for(int i=0; i<(SAMPLES/2); i++)
    {
        // View all these three lines in serial terminal to see which frequencies has which amplitudes         
        Serial.println((i * 1.0 * SAMPLING_FREQUENCY) / SAMPLES, 1);
        //Serial.print(" ");
        //Serial.println(vReal[i], 1);    //View only this line in serial plotter to visualize the bins
    }
    */
    Serial.println(String(amp) + "\t time: " + String(( micros() - Now )));     //Print out what frequency is the most dominant.
    //delay(100);
    
}
