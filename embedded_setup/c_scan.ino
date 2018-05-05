void scan_fcn(){
  Serial.println("Inside scan_fcn");
	while (find_distance_ir() > 50){
  Serial.println("Left turn");
		left();
    delay(100);
	}
  Serial.println("Found beacon");
	motorStop();
	key = found_beacon_k;

}
