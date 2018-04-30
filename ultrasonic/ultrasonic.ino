/* Basic HC-SR04 Code
 * 
 * Uses median filtering to get a more accurate value
 * 
 * 
 * Vcc: 5V
 * Trig: Pin 11
 * Echo: Pin 12
 * Ground: Ground
 * 
 */

int trigPin = 11;
int echoPin = 12;
int numMed = 11;
long duration;
float distance;
float measurements[11];

void setup() {
  // put your setup code here, to run once:
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);

}

/*
 *  This is for the median filtering
 */
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

void loop() {

  for (int i = 0; i < numMed; i++){

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
      delay(50);
    }
  }

  // take median as true distance
  sort(measurements, numMed);
  Serial.print("Distance:  ");
  Serial.println(measurements[numMed/2]);

}
