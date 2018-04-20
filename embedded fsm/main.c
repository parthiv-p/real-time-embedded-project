//
//  main.c
//  embedded
//
//  Created by Ian Jacobsen on 4/15/18.
//  Copyright © 2018 Ian Jacobsen. All rights reserved.
//

#include "taskFunctions.h"
#include "fsm.h"
#include <string.h>
#include <stdio.h>

// globals
int offFlag = 0;
int newData = 1;

extern key keyInput;
extern state presentState;

extern const char* keyNames[];
extern const char* stateNames[];

// only for testing
key testingKeyArray[] = {
    pushButton,
    toneFound,
    spinComplete,
    getFeedback,
    notFeedbackGood,
    finished,
};


int main(int argc, const char * argv[]) {

    printf("starting program........ \n");

    //while (!offFlag){
    for (int i = 0; i < 6; i++){
        if(newData){
            newData = 0;
            printf("==================================================== \n");
            printf("present_state = %s \n", stateNames[presentState]);
            printf("key input = %s \n", keyNames[keyInput]);
            fsm(presentState, keyInput);

        }
    }
}
