void drive_straight_fcn(){  

  delay(3000);

  // drive motors
  
  // check proximity repeatedly
  while(1){
    if (find_distance() < 7){
      key = danger_zone_k;
      break;
    }
  }
}
