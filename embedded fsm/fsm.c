#include "fsm.h"

/*
 
 the FSM flow is:
 
    find input; execute task; move to next state
 
 */

// initial state
state presentState = idle;

// check if there is a change in state


const char* stateNames[] = {"idle",
                            "search",
                            "drive",
                            "spin",
                            "feedback",
                            "stop"};


const char* keyNames[] = {"pushButton",
                          "dangerZone",
                          "toneFound",
                          "notToneFound",
                          "feedbackGood",
                          "notFeedbackGood",
                          "getFeedback",
                          "spinComplete",
                          "notFinished",
                          "finished",
                          "eol"};


const transition idle_transitions[] = {
    // input            next state      task
    {pushButton,        search,         search_fcn},
    {eol,               idle,           null_fcn}
};

const transition search_transitions[] = {
    // input            next state      task
    {toneFound,         spin,           spin_fcn},
    {notToneFound,      drive,          drive_fcn},
    {eol,               search,         null_fcn}
};

const transition drive_transitions[] = {
    // input            next state      task
    {getFeedback,       feedback,       feedback_fcn},
    {dangerZone,        stop,           stop_fcn},
    {eol,               drive,          null_fcn}
};

const transition spin_transitions[] = {
    // input            next state      task
    {spinComplete,      drive,          drive_fcn},
    {eol,               spin,           null_fcn}
};

const transition feedback_transitions[] = {
    // input            next state      task
    {feedbackGood,      drive,          drive_fcn},
    {notFeedbackGood,   stop,           stop_fcn},
    {eol,               feedback,       null_fcn}
};

const transition stop_transitions[] = {
    // input            next state      task
    {notFinished,       search,         search_fcn},
    {finished,          idle,           null_fcn},
    {eol,               stop,           null_fcn}
};

const transition* ps_transitions_ptr[]={
    idle_transitions,
    search_transitions,
    drive_transitions,
    spin_transitions,
    feedback_transitions,
    stop_transitions

};

void fsm (state ps, key keyval){
    int i;

    // find the input key
    for(i = 0; (ps_transitions_ptr[ps][i].keyval != keyval) && (ps_transitions_ptr[ps][i].keyval != eol); i++);

    // execute the associated task
    ps_transitions_ptr[ps][i].tf_ptr();

    // jump to next state
    presentState = ps_transitions_ptr[ps][i].next_state;
}
