// NOTE: if we don't have a button to press, we can simply jump 5V to pin 15 to simulate a button press

uint8_t prev_button_state = LOW;

float check_button(){
  uint8_t button_state = digitalRead(button_pin);
  if (button_state == LOW && prev_button_state == HIGH ){
    return 1;
  } else{
    return 0;
  }
  prev_button_state = button_state;
}

