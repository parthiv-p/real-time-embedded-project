int trigPin = 11;
int echoPin = 12;
int numMed = 4;
long duration;
float measurements[4];

void setup(){
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    Serial.begin(9600);

    FTM0_MODE |= (1U << 2) | (1U << 1); //Mode register

    PORTD_PCR4 |= (1U << 10); // mux to FTM0_C4 for pin 6 - left
    PORTD_PCR5 |= (1U << 10); // mux to FTM0_C5 for pin 20 - right
    FTM0_SC |= (1U << 3) | (1U << 2); //This selects system clock and 16 prescalar

    FTM0_C4SC |= (1U << 3) | (1U << 5); // channel 4
    FTM0_C5SC |= (1U << 3) | (1U << 5); // channel 5
    FTM0_CNTIN = 0; //Initial value is 0 for PWM counter, also edge align mode MUST use CNTIN by design
    FTM0_MOD = 29999;//counts up to MOD then counts back up from 0, this is the period of cycle
}

void reverse(){
    FTM0_C4V = 1500;
    FTM0_C5V = 1500;
}

void forward(){
    FTM0_C4V = 3000;
    FTM0_C5V = 3000;
}

void left(){
    FTM0_C4V = 1500;
    FTM0_C5V = 3000;
}

void right(){
    FTM0_C4V = 3000;
    FTM0_C5V = 1500;
}

void stop(){
    FTM0_C4V = 2250;
    FTM0_C5V = 2250;
}

void swap(float *p,float *q) {
   int t;
   
   t=*p; 
   *p=*q; 
   *q=t;
}
void sort(float a[],int n) { 
   int i,j,temp;

   for(i=0;i<n-1;i++) {
      for(j=0;j<n-i-1;j++) {
         if(a[j]>a[j+1])
            swap(&a[j],&a[j+1]);
      }
   }
}

float get_distance(){
        for (int i = 0; i < numMed; i++){
        float distance;

        // take measurement
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);

        // calculate distance
        duration = pulseIn(echoPin, HIGH, 100000);
        distance = duration * 0.034/2;
      

        // collect 11 measurements
        if ((distance < 500) && (distance != 0)){
          measurements[i] = distance;
          //Serial.print("Distance: ");
          //Serial.println(distance);
          //delay(50);
        }
      }

      // take median as true distance
      sort(measurements, numMed);
      return measurements[numMed/2];
}

void loop() {
    while (get_distance() > 20){
//        Serial.print("Distance:  ");
//        Serial.println(distance);
        forward();
    }
    stop();
}
