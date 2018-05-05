SharpIR sensor(GP2Y0A21YK0F, 21);


int find_distance_ir(){
    int distance =  (sensor.getDistance() * 30 / 18);
    Serial.println("Distance:" + String(distance));
    return distance;
  }
