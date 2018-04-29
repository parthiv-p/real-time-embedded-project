typedef enum{
    idle_s,
    drive_straight_s
} state_t;

typedef enum{
    start_k,
    danger_zone_k,
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
    {drive_straight_s,      danger_zone_k,       idle_s},
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
