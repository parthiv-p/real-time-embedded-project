void drive_straight_fcn(){  

  delay(3000);

  // drive motors
  
  // check proximity repeatedly
  while(find_distance() < 7){
    forward(); //from b_motors
  }
  key = danger_zone_k;
}
