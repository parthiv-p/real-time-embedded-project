#define LAST_BEACON_THRESHOLD 150.0f

void fingerprint_fcn(){

  // Arduino FFT lib implementation
  
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
}

