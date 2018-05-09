#define STRAIGHT_TIMEOUT 5000

void drive_straight_fcn(){  
  
  delay(100);
  unsigned long straight_time = millis();
  // drive motors
  // check proximity repeatedly
  while(1){

    
    forward(); //from b_motors
    // Key check
    if(find_distance_ir() < 20) // prevent entering danger zone for testing w/o ultrasonic
    {
      Serial.println("Entered danger zone");
      motorStop();
      key = danger_zone_k;
      break;
    }

    if(operation_mode == 0)
    {
      if(millis() - straight_time > STRAIGHT_TIMEOUT)
      {
        motorStop();
        key = straight_timeout_k;
        break;
      }
    } else {
      if(millis() - straight_time > 4*STRAIGHT_TIMEOUT)
      {
        motorStop();
        key = straight_timeout_k;
        break;
      }
    }
  }

  if(operation_mode == 1)
    operation_mode = 0;
}
