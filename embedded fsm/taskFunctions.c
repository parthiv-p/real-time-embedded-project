#include "taskFunctions.h"
#include "fsm.h"
#include <stdio.h>

key keyInput;
extern int newData;
extern int offFlag;

// ~~~ JUST FOR TESTING ~~~
extern key testingKeyArray[];

/*
 key testingKeyArray[] = {
 pushButton,
 toneFound,
 spinComplete,
 getFeedback,
 notFeedbackGood,
 finished
 };

 states:
 idle -> pushButton           0
 search -> toneFound          1
 spin -> spinComplete         2
 drive -> getFeedback         3
 feedback -> notFeedbackGood  4
 stop -> finished             5
 */


void search_fcn(){
    /*
     tasks:
        - spin 360 degrees
        - take FFT
        - find best angle
     */
    printf("rotating 360 degrees, searching for beacon \n");
    keyInput = testingKeyArray[1];
    newData = 1;
}

void spin_fcn(){
    /*
     tasks:
        - spin X degrees
     */
    printf("spinning some degrees \n");
    keyInput = testingKeyArray[2];
    newData = 1;
}

void drive_fcn(){
    /*
     tasks:
        - drive straight
     */
    printf("driving straight \n");
    keyInput = testingKeyArray[3];
    newData = 1;
}

void feedback_fcn(){
    /*
     tasks:
        - take FFT
        - make sure that magnitude of desired tone is increasing
     */
    printf("checking feedback \n");
    keyInput = testingKeyArray[4];
    newData = 1;
}

void stop_fcn(){
    /*
     tasks:
        - stop motors
     */
    printf("stopping motors \n");
    keyInput = testingKeyArray[5];
    newData = 1;
}

void null_fcn(){
    //printf("executed null function \n");
    newData = 1;
}
