//
// Created by worrum on 19.09.2022.
//

#ifndef MODELOFOPERATIONOFTHEPNEUMATICCYLINDER_PNEUMOHEADER_H
#define MODELOFOPERATIONOFTHEPNEUMATICCYLINDER_PNEUMOHEADER_H

#include <stdbool.h>

enum PneumoState {
    //PneumoState_Init = 0,
    PneumoState_0, PneumoState_1, PneumoState_2,
    PneumoState_3, PneumoState_4, PneumoState_5,
    PneumoState_6, PneumoState_7, PneumoState_8,
    PneumoState_9, PneumoState_10, PneumoState_11,
    PneumoState_12, PneumoState_13, PneumoState_14,
    PneumoState_15, PneumoState_16, PneumoState_17,
    PneumoState_FatalException
};

#define PNEUMO_CYLINDER_SIGNAL_UP 0
#define PNEUMO_CYLINDER_SIGNAL_DOWN 1

struct PneumoCylinder {
    int input_signal[2];
    int output_signal;
};

#define PNEUMO_CYLINDER_Y1 0
#define PNEUMO_CYLINDER_Y2 1
#define PNEUMO_CYLINDER_Y3 2
#define PNEUMO_CYLINDER_Y4 3
#define PNEUMO_CYLINDER_Y5 4
#define PNEUMO_CYLINDER_Y6 5
#define PNEUMO_CYLINDER_Y7 6
#define PNEUMO_CYLINDER_Y8 7

struct PneumoEngine {
    enum PneumoState state;
    int timeout;
    int delay;
    int timeouts[PneumoState_FatalException];
    int delays[PneumoState_FatalException];
    struct PneumoCylinder cylinders[8];
};
//функции автомата
__attribute__((unused)) void pneumoEngineInit(struct PneumoEngine *engine);

bool pneumo_engine_tick(struct PneumoEngine *engine);

void pneumo_error_handler(struct PneumoEngine *engine, enum PneumoState state);

void delay_timeout_init(struct PneumoEngine *engine, int delta_t, int delta_d, enum PneumoState state);

void pneumo_state_body(struct PneumoEngine *engine, int cylinderSignals[], enum PneumoState next_state, enum PneumoState error_state);

void pneumo_state_changer(struct PneumoEngine* engine, int cylinderSignals[]);

bool pneumo_state_checker(struct PneumoEngine* engine, int cylinderSignals[]);

#endif //MODELOFOPERATIONOFTHEPNEUMATICCYLINDER_PNEUMOHEADER_H