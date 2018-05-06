#define BLINK_TIME 250

void idle_fcn(){

  unsigned long Now = millis();
  int led_state = -1;
  delay(3000);

  // check proximity repeatedly
  while(1){
    if(millis() - Now > BLINK_TIME)
    {
      led_state *= -1;
      if(led_state > 0)
        digitalWrite(led_pin, HIGH);
      else if(led_state < 0)
        digitalWrite(led_pin, LOW);
    }
    
    if (check_button()){
      key = start_k;
      digitalWrite(led_pin, LOW);
      break;
    }
  }
}

