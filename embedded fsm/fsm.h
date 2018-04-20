#ifndef FSM
#define FSM

#include "taskFunctions.h"

typedef enum {idle,
              search,
              drive,
              spin,
              feedback,
              stop} state;

typedef enum {pushButton,
              dangerZone,
              toneFound,
              notToneFound,
              feedbackGood,
              notFeedbackGood,
              getFeedback,
              spinComplete,
              notFinished,
              finished,
              eol} key;

typedef void (*task_fcn_ptr)();

typedef struct {
    key keyval;
    state next_state;
    task_fcn_ptr tf_ptr;
} transition;

void fsm(state ps, key keyval);
 
#endif

