/*
 * - the beacons have a period of 2s
 * - we take 512 samples at 22kHz... taking 512 * 1/22k = 0.0233 seconds
 * - 0.0233s * 20 = 0.47s
 * - 2s - 0.47s = 1.53s
 * - 1.53s / 20 = 0.077s = 77ms
 * - so inbetween our 20 samples, we delay 77ms
 */

// FFT setup
arduinoFFT FFT = arduinoFFT(); 
const uint16_t samples = 512;
const double samplingFrequency = 22000;
uint16_t sampling_period_us = uint16_t(1000000*(1.0/samplingFrequency));
unsigned long microseconds;
double vReal[samples];
double vImag[samples];

// beacon detection setup
uint16_t closest;
const uint16_t bin_sz = 150; // 150Hz total... 75Hz on each side
uint8_t idx;
uint16_t temp_finger_print[10] = {0};
uint16_t finger_print[10] = {0};


// bins all counts within a 75Hz window on either side of the target tone (simplified histogram)
void bin_results(){
  double frq;
  uint16_t buffer_size = (samples << 1);

  // reset tones array
  for (uint16_t i = 0; i < 10; i++){
    temp_finger_print[i] = 0;
  }

  for (uint16_t i = 0; i < buffer_size; i++){
    
    // calculate frequency associate with entry number i
    frq = ((i * 1.0 * samplingFrequency) / samples);

    // if we are close to a target tone, add the count to the simplified histogram
    for (uint8_t j = 0; j < 10; j++){
      if (abs(tone_list[j] - frq) < bin_sz/2) {
        temp_finger_print[j] = temp_finger_print[j] + vReal[i];
      }
    }
    
  }
}

void add_results(){
  // add histograms
  for (uint8_t i = 0; i < 10; i++){
    finger_print[i] = finger_print[i] + temp_finger_print[i];
  }
  
}

// returns the tone of the beacon with the most dominant signal (hopefully the closest)
uint16_t get_closest_beacon_index(){
  uint16_t max_pos = 0;
  uint16_t max_bin = 0;
  for (uint8_t i = 0; i < 10; i ++){
    if (finger_print[i] > max_bin){
      max_bin = finger_print[i];
      max_pos = i;
    }
  }

  Serial.print("Tone: ");
  Serial.print(tone_list[max_pos]);
  Serial.print("    Count: ");
  Serial.println(finger_print[max_pos]);
  return max_pos;
}


void get_fingerprint(){

  // reset fingerprint
  for (uint8_t i = 0; i < 10; i++){
    finger_print[i] = 0;
  }

  // uniformly sample every 73ms over 2 second period
  for(uint8_t j = 0; j < 20; j++){
  
    // sample ADC
    for(uint16_t i=0; i<samples; i++)
    {
        // this is a bad way to sample... change this to interrupt driven and don't use analogRead
        microseconds = micros();    //Overflows after around 70 minutes!
        vReal[i] = analogRead(tone_pin);
        vImag[i] = 0;
        while(micros() < (microseconds + sampling_period_us)); // wait until next sample interval
    }
    
    // do FFT
    FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
    FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
    FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */
  
    // simplify histogram
    bin_results();

    // add finger prints
    add_results();

    // delay
    delay(73);

  }

  // index to closest beacon
  idx = get_closest_beacon_index();
    
}

void print_results(){
  for (uint8_t i = 0; i < 10; i ++){
    Serial.println("");
    Serial.print(tone_list[i]);
    Serial.print("   ");
    Serial.println(finger_print[i]);
  }
  Serial.println("============================");
}
