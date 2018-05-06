// FFT Test
//
// Compute a 1024 point Fast Fourier Transform (spectrum analysis)
// on audio connected to the Left Line-In pin.  By changing code,
// a synthetic sine wave can be input instead.
//
// The first 40 (of 512) frequency analysis bins are printed to
// the Arduino Serial Monitor.  Viewing the raw data can help you
// understand how the FFT works and what results to expect when
// using the data to control LEDs, motors, or other fun things!
//
// This example code is in the public domain.

#include <Audio.h>

const int myInput = AUDIO_INPUT_LINEIN;
unsigned long Now, readTimer;

AudioInputAnalog          audioInput;         // audio shield: mic or line-in
//AudioAnalyzeFFT1024    myFFT;
AudioAnalyzeToneDetect   tones[10];

//AudioConnection patchCord1(audioInput, 0, myFFT, 0);
AudioConnection patchCord2(audioInput, tones[0]);
AudioConnection patchCord3(audioInput, tones[1]);
AudioConnection patchCord4(audioInput, tones[2]);
AudioConnection patchCord5(audioInput, tones[3]);
AudioConnection patchCord6(audioInput, tones[4]);
AudioConnection patchCord7(audioInput, tones[5]);
AudioConnection patchCord8(audioInput, tones[6]);
AudioConnection patchCord9(audioInput, tones[7]);
AudioConnection patchCord10(audioInput, tones[8]);
AudioConnection patchCord11(audioInput, tones[9]);

void setup() {

  AudioMemory(12);
  for(int i = 0 ; i < 10 ; i++)
  {
    tones[i].frequency(5000+500*i, 500);
  }
  //myFFT.windowFunction(AudioWindowHanning1024);
}

void loop() {
  float mags[10] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
  //int counter = 0;
  Now = millis();
  while(millis() - Now < 1250)
  {
    readTimer = micros();
    for(int i = 0 ; i < 10 ; i++)
    {
      mags[i] += tones[i].read();
    }
    //counter++;
  }
  //Serial.print("Count: " + String(counter));
  for(int i = 0 ; i < 10 ; i++)
  {
    Serial.print("\t" + String(mags[i]));
  }
  Serial.print("\n");

}


