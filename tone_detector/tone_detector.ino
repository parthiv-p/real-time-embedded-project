#include "arduinoFFT.h"

// hardware setup
uint8_t channel_pin = A2; // analog input 0 (pin 14)

// FFT setup
arduinoFFT FFT = arduinoFFT(); 
const uint16_t samples = 512;
const double samplingFrequency = 20000;
unsigned int sampling_period_us;
unsigned long microseconds;
double vReal[samples];
double vImag[samples];
const int numReads = 50;

// beacon detection setup
uint16_t closest;
const uint16_t bin_sz = 150; // 150Hz total... 75Hz on each side
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
uint16_t target_tones[10] = {0};


void setup()
{

  // sampling period in microseconds
  sampling_period_us = round(1000000*(1.0/samplingFrequency));

  // set up for communication with serial port
  Serial.begin(115200);
}

void loop()
{

  // loop numReads number of times
  for(int j = 0; j < numReads; j++){
    
    // sample ADC
    for(int i=0; i<samples; i++)
    {
        // this is a bad way to sample... change this to interrupt driven and don't use analogRead
        microseconds = micros();    //Overflows after around 70 minutes!
        vReal[i] = analogRead(channel_pin);
        vImag[i] = 0;
        while(micros() < (microseconds + sampling_period_us)); // wait until next sample interval
    }
    
    // do FFT
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */

    // find beacon
    bin_results(vReal, (samples >> 1));
    closest = get_closest_beacon();
    print_results();

    // wait 1 seconds
    delay(1000);

  }
  while(1);
}


// bins all counts within a 75Hz window on either side of the target tone (simplified histogram)
void bin_results(double *vData, uint16_t bufferSize){
  double frq;

  // reset tones array
  for (uint16_t i = 0; i < 10; i++){
    target_tones[i] = 0;
  }

  for (uint16_t i = 0; i < bufferSize; i++){
    
    // calculate frequency associate with entry number i
    frq = ((i * 1.0 * samplingFrequency) / samples);

    // if we are close to a target tone, add the count to the simplified histogram
    for (uint8_t j = 0; j < 10; j++){
      if (abs(tone_list[j] - frq) < bin_sz/2) {
        target_tones[j] = target_tones[j] + vData[i];
      }
    }
    
  }
}

void print_results(){
  Serial.println("============================");
  for (uint8_t i = 0; i < 10; i ++){
    Serial.print(tone_list[i]);
    Serial.print("   ");
    Serial.println(target_tones[i]);
  }
}

// returns the tone of the beacon with the most dominant signal (hopefully the closest)
uint16_t get_closest_beacon(){
  uint16_t max_pos = 0;
  uint16_t max_bin = 0;
  for (uint8_t i = 0; i < 10; i ++){
    if (target_tones[i] > max_bin){
      max_bin = target_tones[i];
      max_pos = i;
    }
  }

  Serial.print("Closest beacon is:  ");
  Serial.print(tone_list[max_pos]);
  Serial.println("Hz");

  return tone_list[max_pos];
}


