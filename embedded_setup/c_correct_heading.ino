void correct_heading_fcn(){  

  delay(1000);
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
      
      if (prev_event == decision_made_k)
      {
        Serial.println("Timeout for decision turning");
        key = dec_timeout_k;
        break;
      } 
      
      /*
      else if (prev_event == timeout_k)
      {
        Serial.println("Timeout for straight movement correction");
        key = fwd_timeout_k;
        break;
      }
      */
    }  
  }

  Serial.println("Exited correct heading");
}
