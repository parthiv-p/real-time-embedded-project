

void loop(){
    
    switch(current_state){
        case idle_s:
            // look for "start" key
            Serial.println("in idle state");
            idle_fcn();
            break;
        case drive_straight_s:
            Serial.println("driving straight");
            drive_straight_fcn();
            break;
        case fingerprint_s:
            Serial.println("fingerprinting");
            fingerprint_fcn();
            break;
        case scan_s:
            Serial.println("scanning");
            scan_fcn();
            break;
        case beacon_avoid_s:
            Serial.println("avoiding beacon");
            beacon_avoid_fcn();
            break;
    }

    // when a new event occurs, update state
    update_state();
}

