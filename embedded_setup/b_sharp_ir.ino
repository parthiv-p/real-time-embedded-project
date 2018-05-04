SharpIR sensor(GP2Y0A21YK0F, 21);

uint8_t num_med_ir = 5;
int measurements_ir[5];

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
   for (int i = 0; i < num_med_ir; i++){
    measurements_ir[i] = sensor.getDistance();
    delayMicroseconds(15);
   }

  sort_ir(measurements_ir, num_med_ir);
  return measurements_ir[num_med_ir/2];
}
