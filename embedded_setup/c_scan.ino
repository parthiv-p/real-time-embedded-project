#define SCAN_THRESH 80
#define OBJECT_CONFIDENCE 25
#define FAULT_LIMIT 5

void scan_fcn(){
  int count = 0, fault_count = 0;
  int prev = 90;
  delay(100);
    
	while (1){
		right();  // CW scan
    sensorRead();
    int dist = find_distance_ir();
    if(dist < SCAN_THRESH && prev < SCAN_THRESH)
    {
      count++;
    } else {
      count = 0;
    }
    prev = dist;
    if(count > OBJECT_CONFIDENCE)
    {
      Serial.println("IR object detected");
      motorStop();
      float ultrasonic_dist = find_distance();
      if (ultrasonic_dist < SCAN_THRESH && ultrasonic_dist > 2.0f)
      {
        Serial.println("Ultrasonic object detected");
        break;
      } else {
        Serial.println("Ultrasonic rejects the obj");
        count = 0;
        fault_count++;
        continue;
      }
    }
    if(fault_count > FAULT_LIMIT)
    {
      reset_quaternion();
      refCommand = PI / 16.0f;
      correct_heading();
      fault_count = 0;
    }
	}
 
  Serial.println("found beacon");
	motorStop();
  delay(100);
  reset_quaternion();
	key = found_beacon_k;

}
