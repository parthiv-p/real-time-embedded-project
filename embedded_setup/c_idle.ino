void idle_fcn(){

  delay(3000);

  // check proximity repeatedly
  while(1){
    if (check_button()){
      key = start_k;
      break;
    }
  }
  
}

