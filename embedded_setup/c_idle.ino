#define BLINK_TIME 250

void idle_fcn(){

  unsigned long Now = millis(), debounce_time = 0, debounce_delay = 50;
  int led_state = LOW;
  int reading, button_state = LOW, prev_button_state = LOW;
  
  delay(2000);

  // check button press repeatedly
  while(1){
    //int test = find_distance_ir();
    //Serial.println(String(test));
    if(millis() - Now > BLINK_TIME)
    {
      led_state = !led_state;
      digitalWrite(led_pin, led_state);
      Now = millis();
    }
    
    reading = digitalRead(button_pin);
    if(reading != prev_button_state)
    {
      debounce_time = millis();
    }

    if((millis() - debounce_time) > debounce_delay)
    {
      if(reading != button_state)
      {
        button_state = reading;  
        if(button_state == LOW)
        {
          Serial.println("Button pressed");
          key = start_k;
          digitalWrite(led_pin, LOW);
          break;
        }
      }
    }
    prev_button_state = reading;
  }
  delay(3000);
}

