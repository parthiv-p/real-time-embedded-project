typedef enum{
    idle_s,
    drive_straight_s,
    beacon_avoid_s,
    fingerprint_s,
    scan_s
} state_t;

typedef enum{
    start_k,
    danger_zone_k,
    last_beacon_k,
    not_done_k,
    found_beacon_k,
    settle_time_k,
    complete_k,
    eol_k
}event_t;

typedef struct{
    state_t current;
    event_t key;
    state_t next;
}state_transition_t;

const state_transition_t transition_table[] = {
    // current state        event                next state
    {idle_s,                start_k,             drive_straight_s},
    {drive_straight_s,      danger_zone_k,       fingerprint_s},
    {fingerprint_s,         last_beacon_k,       idle_s},
    {fingerprint_s,         not_done_k,          beacon_avoid_s},  // beacon avoid 2 cases - distance (turn 45 CW) or timeout (turn 90 CCW) 
    {beacon_avoid_s,        complete_k,          scan_s},
    {scan_s,                found_beacon_k,      drive_straight_s},
    {idle_s,                eol_k,               idle_s}              // must be the last entry in table
};

// initial state
state_t current_state = idle_s;

// global key
event_t key;

// update state based on key
void update_state(){
    int i;
    while(transition_table[i].key != eol_k){
        if ((current_state == transition_table[i].current) &&
            (key == transition_table[i].key)){
            current_state = transition_table[i].next;
            break;
        }
        i++;
    }
}
