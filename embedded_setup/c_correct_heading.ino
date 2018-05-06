void correct_heading_fcn(){  

  delay(100);
  stateTime = millis();
  event_t prev_event = key;
        
  while(1)
  {
    turn();
    //Serial.println("Executed turn");
    if(millis() - stateTime >= settleTime)
    {
      motorStop();
      reset_quaternion();
      refCommand = 0;
      key = settle_time_k; // change
      break;
    }  
  }

  Serial.println("Exited correct heading");
}
