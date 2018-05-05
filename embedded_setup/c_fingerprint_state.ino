void fingerprint_fcn(){

  delay(3000);

  // check if we are at last tone
  get_fingerprint();

  Serial.println(tone_list[idx]);
  //print_results(); // this can help with debugging

  // if we are at the last beacon
  if (tone_list[idx] == tone_list[9]){
    digitalWrite(13, HIGH); // light LED if we are finished
    key = last_beacon_k;
  }
  // otherwise scan and find a different beacon
  else{
    key = not_done_k;
  }
}

