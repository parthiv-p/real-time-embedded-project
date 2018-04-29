void idle_fcn(){

  delay(3000);

  // check proximity repeatedly
  while(1){
    if (find_distance() < 5){
      key = start_k;
      break;
    }
  }
  
}

