#define SCAN_THRESH 175
#define OBJECT_CONFIDENCE 25

void scan_fcn(){
  int count = 0;
  int prev = 80;
  delay(100);
    
	while (1){
		right();  // CW scan
    sensorRead();
    int dist = find_distance_ir();
    if(dist > SCAN_THRESH && prev > SCAN_THRESH)
    {
      count++;
    } else {
      count = 0;
    }
    prev = dist;
    if(count > OBJECT_CONFIDENCE)
      break;
	}
 
  Serial.println("found beacon");
	motorStop();
  delay(100);
  reset_quaternion();
	key = found_beacon_k;

}
