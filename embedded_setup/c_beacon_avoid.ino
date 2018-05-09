#define FWD_TIMEOUT 2300 // tbd experimentally

void beacon_avoid_fcn(){  
  
  delay(100);

  // execute turn
  refCommand = PI/4; // 45 deg CW turn
  correct_heading();
  unsigned long Now = millis();
  
  while(1)
  {
    forward();
    if(find_distance_ir() < 20) // encountered an object before reaching final pos
    {
      motorStop();
      refCommand = PI/4; // 45 deg CW turn
      correct_heading();
      break;
    }
    if (millis() - Now > FWD_TIMEOUT)
    {
      motorStop();
      refCommand = -1.0*PI/2; // 90 deg CCW turn
      correct_heading();
      break;
    }  
  }
  
  key = complete_k;
  Serial.println("beacon avoid completed");
}
