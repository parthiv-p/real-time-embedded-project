#define VAR_THRESHOLD 250.0f

SharpIR sensor(GP2Y0A21YK0F, 21);

const int num_med_ir = 10;
int measurements_ir[num_med_ir];

void swap_ir(int *p,int *q) {
   int t;
   t=*p; 
   *p=*q; 
   *q=t;
}
void sort_ir(int a[],int n) { 
   uint8_t i,j;
   for(i=0;i<n-1;i++) {
      for(j=0;j<n-i-1;j++) {
         if(a[j]>a[j+1])
            swap_ir(&a[j],&a[j+1]);
      }
   }
}

int find_distance_ir(){
  float avg = 0, var = 0;
  for (int i = 0; i < num_med_ir; i++){
   int dist = (sensor.getDistance()*30/18);
   measurements_ir[i] = dist;
   avg += dist;
   Serial.print(String(dist) + "\t");
   delayMicroseconds(15);
  }

  avg /= (float)num_med_ir;
  for(int i = 0 ; i < num_med_ir ; i++)
  {
    var += ((float)measurements_ir[i] - avg)*((float)measurements_ir[i] - avg);
  }
  if(var > VAR_THRESHOLD)
  {
    Serial.println("Dist: " + String(135));
    return 135;
  }

  sort_ir(measurements_ir, num_med_ir);
  Serial.println("Dist: " + String(measurements_ir[num_med_ir / 2]));
  return measurements_ir[num_med_ir/2];
}
