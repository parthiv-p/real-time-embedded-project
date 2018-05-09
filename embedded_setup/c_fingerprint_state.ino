#define LAST_BEACON_THRESHOLD 200.0f
#define TRIALS 3

void fingerprint_fcn(){
  digitalWrite(led_pin, LOW);
  // Arduino FFT lib implementation - only 9.5k
  int counts = 0;

  for(int i = 0 ; i < TRIALS ; i++)
  {
    float amp = check_freq();
    if(amp > 2*LAST_BEACON_THRESHOLD)
    {
      counts = 2;
      break;  
    } else if(amp > LAST_BEACON_THRESHOLD)
    {
      counts++;
    }
    if(counts >= 2)
      break;
  }

  if(counts > 1)
  {
    Serial.println("Beacon found");
    key = last_beacon_k;
    digitalWrite(led_pin, HIGH);
  } else {
    Serial.println("Beacon not found");
    key = not_done_k;
    digitalWrite(led_pin, LOW);
  }

  /*

  // Arduino FFT lib implementation - all frequencies
  
  //delay(3000);
  // check if we are at last tone
  get_fingerprint();
  for(int i = 0 ; i < 10 ; i++)
  {
    Serial.print(String(finger_print[i]) + "\t");
  }
  Serial.println(" ");
  //print_results(); // this can help with debugging

  // if we are at the last beacon
  if(finger_print[9] > LAST_BEACON_THRESHOLD) //if (tone_list[idx] == tone_list[9])
  {
    digitalWrite(13, HIGH); // light LED if we are finished
    //key = last_beacon_k;
  }
  // otherwise scan and find a different beacon
  else{
    digitalWrite(13, LOW);
    //key = not_done_k;
  }
  */
}

