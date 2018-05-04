typedef enum{
    idle_s,
    drive_straight_s,
    drive_backward_s,
    correct_heading_s,
    determine_action_s
} state_t;

typedef enum{
    start_k,
    danger_zone_k,
    timeout_k,
    fwd_timeout_k,
    dec_timeout_k,
    decision_made_k,
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
    {drive_straight_s,      danger_zone_k,       determine_action_s},
    {drive_straight_s,      timeout_k,           correct_heading_s},
    {correct_heading_s,     fwd_timeout_k,       determine_action_s},
    {correct_heading_s,     dec_timeout_k,       drive_straight_s},
    {determine_action_s,    decision_made_k,     correct_heading_s},
    {idle_s,                eol_k,               idle_s}              // must be the last entry in table
};

// initial state
state_t current_state = correct_heading_s;  // changed for testing

// global key
event_t key = decision_made_k; // assigned a value for testing only

// update state based on key
void update_state(){
    int i = 0;
    while(transition_table[i].key != eol_k){
        if ((current_state == transition_table[i].current) &&
            (key == transition_table[i].key)){
            current_state = transition_table[i].next;
            
            break;
        }
        i++;
    }
    Serial.println("State updated");
}
