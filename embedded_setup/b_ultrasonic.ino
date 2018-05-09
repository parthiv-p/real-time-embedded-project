float duration;
float distance;
uint8_t num_med = 10;
float measurements[10];

void swap(float *p,float *q) {
   float t;
   t=*p; 
   *p=*q; 
   *q=t;
}
void sort(float a[],int n) { 
   uint8_t i,j;
   for(i=0;i<n-1;i++) {
      for(j=0;j<n-i-1;j++) {
         if(a[j]>a[j+1])
            swap(&a[j],&a[j+1]);
      }
   }
}

float find_distance(){
   for (int i = 0; i < num_med; i++){

    // take measurement
    digitalWrite(trig_pin, LOW);
    delayMicroseconds(2);
    digitalWrite(trig_pin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig_pin, LOW);
    duration = pulseIn(echo_pin, HIGH, 100000);

    // calculate distance
    distance = duration * 0.034/2;
  
    measurements[i] = distance;
    delay(5);
   }

  sort(measurements, num_med);
  Serial.println("Ultrasonic dist: " + String(measurements[num_med/2]));
  return measurements[num_med/2];
}
