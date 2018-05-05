// NOTE: if we don't have a button to press, we can simply jump 5V to pin 15 to simulate a button press

float check_button(){
  uint8_t button_state = digitalRead(button_pin);
  if (button_state == HIGH){
    return 1;
  } else{
    return 0;
  }
}

