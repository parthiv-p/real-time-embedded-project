
// connect piezo pin with dot above it to pin 10, other pin to ground

// use a jumper wire from 5V to pins 0-9 to generate tones

// tones are on for 1.25s, off for 0.75s

// only one tone at a time

#define piezoPin 10
#define k5 0
#define k55 1
#define k6 2
#define k65 3
#define k7 4
#define k75 5
#define k8 6
#define k85 7
#define k9 8
#define k95 9

uint16_t freq;

void setup() {

  pinMode(k5, INPUT_PULLDOWN);
  pinMode(k55, INPUT_PULLDOWN);
  pinMode(k6, INPUT_PULLDOWN);
  pinMode(k65, INPUT_PULLDOWN);
  pinMode(k7, INPUT_PULLDOWN);
  pinMode(k75, INPUT_PULLDOWN);
  pinMode(k8, INPUT_PULLDOWN);
  pinMode(k85, INPUT_PULLDOWN);
  pinMode(k9, INPUT_PULLDOWN);
  pinMode(k95, INPUT_PULLDOWN);
 
}
 
void loop() {

  if (digitalRead(k5)){
    freq = 5000;
  }
  else if(digitalRead(k55)){
    freq = 5500;
  }
  else if(digitalRead(k6)){
    freq = 6000;
  }
  else if(digitalRead(k65)){
    freq = 6500;
  }
  else if(digitalRead(k7)){
    freq = 7000;
  }
  else if(digitalRead(k75)){
    freq = 7500;
  }
  else if(digitalRead(k8)){
    freq = 8000;
  }
  else if(digitalRead(k85)){
    freq = 8500;
  }
  else if(digitalRead(k9)){
    freq = 9000;
  }
  else if(digitalRead(k95)){
    freq = 9500;
  }
  else{
    freq = 0;
  }
  
  tone(piezoPin, freq);
  delay(1250);
  noTone(piezoPin);
  delay(750);
 
}
