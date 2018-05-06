#define SCAN_THRESH 80
#define OBJECT_CONFIDENCE 50

void scan_fcn(){
  int count = 0;
  int prev = 80;
  delay(100);
  Serial.println("Inside scan_fcn");
  
	while (1){
    Serial.println("Right turn");
		right();
    sensorRead();
    //delay(25);
    int dist = find_distance_ir();
    if(dist < SCAN_THRESH && prev < SCAN_THRESH)
    {
      count++;
    } else {
      count = 0;
    }
    prev = dist;
    if(count > OBJECT_CONFIDENCE)
      break;
	}
 
  Serial.println("Found beacon");
	motorStop();
  refCommand = yaw;
	key = found_beacon_k;

}
