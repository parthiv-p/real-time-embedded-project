void drive_straight_fcn(){  

  delay(1000);
  stateTime = millis();
  
  // drive motors
  
  while(1){
    
    forward(); //from b_motors
    //Serial.println("Executed fwd");
    // Key check
    int dist = find_distance_ir();
    Serial.println("Dist: " + String(dist));
    if(dist < 15) // prevent entering danger zone for testing w/o ultrasonic
    {
      motorStop();
      key = danger_zone_k;
      Serial.println("Danger zone");
      break;
    }
    /*else if(millis() - stateTime >= straightLimit) // for timing test purposes only. Can be reverted back to original
    {
      motorStop();
      key = timeout_k;
      Serial.println("Timeout");
      break; 
    }
    */
  }
}

