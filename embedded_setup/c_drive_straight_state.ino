void drive_straight_fcn(){  

  delay(100);
  
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
  }
}
