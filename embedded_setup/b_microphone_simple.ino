#include "arduinoFFT.h"
 
#define SAMPLES 128           //Must be a power of 2
#define SAMPLING_FREQUENCY 22000 //Hz, must be less than 10000 due to ADC
 
arduinoFFT FFT = arduinoFFT();
 
unsigned int sampling_period_us;
unsigned long microseconds;
 
double vReal[SAMPLES];
double vImag[SAMPLES];

unsigned long sample_time;

float check_freq() {
  
  sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
  unsigned long start_time = millis();
  float amp = 0;
  int samp_count = 0;
  while(millis() - start_time < 1250)
  {
    sample_time = micros();
    for(int i=0; i<SAMPLES; i++)
    {
        microseconds = micros();    //Overflows after around 70 minutes!

        uint16_t data = analogRead(16);
        vReal[i] = (double)data;
        vImag[i] = 0;
     
        while(micros() < (microseconds + sampling_period_us)){
        }
    }

    /*FFT*/
    FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
    if( vReal[55] > 10.0f)
    {
      amp += vReal[55]; //vReal[220] + vReal[221] + vReal[222] for 9.5k, vReal[116] + vReal[117] for 5k  
    }
    
    samp_count++;
  }
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
  Serial.println(String(amp) + "\t count: " + String(samp_count));
  return amp;
}
