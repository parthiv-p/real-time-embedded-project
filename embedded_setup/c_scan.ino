#define SCAN_THRESH 80
#define OBJECT_CONFIDENCE 25
#define FAULT_LIMIT 5
#define BWD_TIMEOUT 1750 
#define SCAN_TIMEOUT 15000

void scan_fcn(){
  int count = 0, fault_count = 0;
  int prev = 90;
  delay(100);

  unsigned long scan_time = millis();
  
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
      if (ultrasonic_dist < SCAN_THRESH && ultrasonic_dist > 1.0f)
      {
        Serial.println("ultrasonic confirm");
        break;
      } else {
        Serial.println("ultrasonic reject");
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
    if(millis() - scan_time > SCAN_TIMEOUT)
    {
      reset_quaternion();
      unsigned long bwd_time = millis();
      while(millis() - bwd_time < BWD_TIMEOUT)
      {
        reverse();
      }
      scan_time = millis();
    }
	}
 
  Serial.println("found beacon");
	motorStop();
  delay(100);
  reset_quaternion();
	key = found_beacon_k;

}
