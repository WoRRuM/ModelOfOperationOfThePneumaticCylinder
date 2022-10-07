#include "pneumoHeader.h"

#define TIMEOUT_DELTA(timeout) ((timeout) * 1000)
#define DELAY_DELTA(delay) ((delay) * 1000)
#define NC -1

void delayTimeoutInit(struct PneumoEngine* engine, int delta_t, int delta_d, enum PneumoState state) {
    engine->timeouts[state] = TIMEOUT_DELTA(delta_t);
    engine->delays[state] = DELAY_DELTA(delta_d);
}

__attribute__((unused)) void pneumoEngineInit(struct PneumoEngine* engine) {
    if (engine != 0) {
        for (int i = 0; i < 8; i++)
        {
            engine->cylinders[i].input_signal[PNEUMO_CYLINDER_SIGNAL_UP] = 0;
            engine->cylinders[i].input_signal[PNEUMO_CYLINDER_SIGNAL_DOWN] = 0;
            engine->cylinders[i].output_signal = 0;
        }

        engine->state = PneumoState_0;
        engine->delay = 0;
        engine->timeout = 0;

        delayTimeoutInit(engine, 45, 70, PneumoState_0);
        delayTimeoutInit(engine, 56, 70, PneumoState_1);
        delayTimeoutInit(engine, 56, 60, PneumoState_2);
        delayTimeoutInit(engine, 45, 78, PneumoState_3);
        delayTimeoutInit(engine, 120, 45, PneumoState_4);
        delayTimeoutInit(engine, 56, 45, PneumoState_5);
        delayTimeoutInit(engine, 120, 33, PneumoState_6);
        delayTimeoutInit(engine, 30, 60, PneumoState_7);
        delayTimeoutInit(engine, 56, 45, PneumoState_8);
        delayTimeoutInit(engine, 60, 45, PneumoState_9);
        delayTimeoutInit(engine, 56, 78, PneumoState_10);
        delayTimeoutInit(engine, 120, 45, PneumoState_11);
        delayTimeoutInit(engine, 56, 70, PneumoState_12);
        delayTimeoutInit(engine, 120, 78, PneumoState_13);
        delayTimeoutInit(engine, 56, 45, PneumoState_14);
        delayTimeoutInit(engine, 30, 60, PneumoState_15);
        delayTimeoutInit(engine, 45, 78, PneumoState_16);
        delayTimeoutInit(engine, 45, 78, PneumoState_17);
    }
}

#define TIMEOUT_GE(engine) ( (engine)->timeout > (engine)->timeouts[(engine)->state] )
#define DELAY_GE(engine) ( (engine)->delay > (engine)->delays[(engine)->state] )

void pneumoErrorHandler(struct PneumoEngine* engine, enum PneumoState state) {
    engine->state = state;
    int cylinderSignals[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    pneumoStateChanger(engine, cylinderSignals);
    engine->delay = 0;
    engine->timeout = 0;
}

void pneumoStateChanger(struct PneumoEngine* engine, const int cylinderSignals[]) {
    for (int i = 0; i < 8; i++)
    {
        switch (cylinderSignals[i])
        {
            case 0: // Cylinder is down
                engine->cylinders[i].output_signal = 0;
                break;
            case 1: // Cylinder is up
                engine->cylinders[i].output_signal = 1;
                break;
            case -1: // Cylinder not changed, doesn't do anything
                break;
        }
    }
}

bool pneumoStateChecker(struct PneumoEngine* engine, const int cylinderSignals[]) {
    for (int i = 0; i < 8; i++)
    {
        switch (cylinderSignals[i])
        {
            case 0: // Cylinder is down
                if (!engine->cylinders[i].input_signal[PNEUMO_CYLINDER_SIGNAL_DOWN])
                    return false;
                break;
            case 1: // Cylinder is up
                if (!engine->cylinders[i].input_signal[PNEUMO_CYLINDER_SIGNAL_UP])
                    return false;
                break;
            case -1: // Cylinder not changed, doesn't do anything
                break;
        }
    }
    return true;
}

void pneumoStateBody(struct PneumoEngine* engine, int cylinderSignals[], enum PneumoState next_state, enum PneumoState error_state) {
    pneumoStateChanger(engine, cylinderSignals);
    if (pneumoStateChecker(engine, cylinderSignals)) {
        engine->timeout = 0;
        engine->delay++;
        if (DELAY_GE(engine)) {
            engine->state = next_state;
            engine->delay = 0;
            engine->timeout = 0;
        }
    }
    else if (TIMEOUT_GE(engine)) {
        pneumoErrorHandler(engine, error_state);
    }
    else if (engine->delay > 0) {
        pneumoErrorHandler(engine, error_state);
    }
}

__attribute__((unused)) bool pneumoEngineTick(struct PneumoEngine* engine) {
    bool ret = true;
    if (0 == engine)
        return false;
    switch (engine->state) {
        case PneumoState_0: {
            int cylinderSignals[] = {0, 0, 0, 0, 0, 0, 0, 0 };
            pneumoStateBody(engine, cylinderSignals, PneumoState_1, PneumoState_FatalException);
            break;
        }
        case PneumoState_1: {
            int cylinderSignals[] = {NC, 1, NC, NC, NC, NC, NC, NC };
            pneumoStateBody(engine, cylinderSignals, PneumoState_3, PneumoState_FatalException);
            break;
        }
        case PneumoState_2: {
            int cylinderSignals[] = {NC, 0, NC, 1, NC, NC, NC, 1 };
            pneumoStateBody(engine, cylinderSignals, PneumoState_4, PneumoState_FatalException);
            break;
        }
        case PneumoState_3: {
            int cylinderSignals[] = {NC, 1, 1, NC, NC, NC, NC, NC };
            pneumoStateBody(engine, cylinderSignals, PneumoState_4, PneumoState_FatalException);
            break;
        }
        case PneumoState_4: {
            int cylinderSignals[] = {1, 0, 0, 0, 1, 1, 1, 0 };
            pneumoStateBody(engine, cylinderSignals, PneumoState_5, PneumoState_FatalException);
            break;
        }
        case PneumoState_5: {
            int cylinderSignals[] = {NC, NC, 1, 1, 0, NC, NC, NC };
            pneumoStateBody(engine, cylinderSignals, PneumoState_6, PneumoState_FatalException);
            break;
        }
        case PneumoState_6: {
            int cylinderSignals[] = {NC, 1, 0, NC, NC, NC, NC, NC };
            pneumoStateBody(engine, cylinderSignals, PneumoState_7, PneumoState_FatalException);
            break;
        }
        case PneumoState_7: {
            int cylinderSignals[] = {NC, 0, NC, NC, NC, NC, NC, 1 };
            pneumoStateBody(engine, cylinderSignals, PneumoState_8, PneumoState_4);
            break;
        }
        case PneumoState_8: {
            int cylinderSignals[] = {0, NC, NC, NC, NC, 0, NC, 0 };
            pneumoStateBody(engine, cylinderSignals, PneumoState_9, PneumoState_12);
            break;
        }
        case PneumoState_9: {
            int cylinderSignals[] = {1, 1, 1, 0, 1, 1, 0, NC };
            pneumoStateBody(engine, cylinderSignals, PneumoState_10, PneumoState_FatalException);
            break;
        }
        case PneumoState_10: {
            int cylinderSignals[] = {0, 0, 0, 1, 0, 0, NC, 1 };
            pneumoStateBody(engine, cylinderSignals, PneumoState_11, PneumoState_FatalException);
            break;
        }
        case PneumoState_11: {
            int cylinderSignals[] = {NC, 1, NC, NC, NC, NC, NC, NC };
            pneumoStateBody(engine, cylinderSignals, PneumoState_12, PneumoState_FatalException);
            break;
        }
        case PneumoState_12: {
            int cylinderSignals[] = {1, NC, NC, 0, 1, NC, NC, 0 };
            pneumoStateBody(engine, cylinderSignals, PneumoState_13, PneumoState_FatalException);
            break;
        }
        case PneumoState_13: {
            int cylinderSignals[] = {0, 0, 1, NC, 0, 1, 1, 1 };
            pneumoStateBody(engine, cylinderSignals, PneumoState_14, PneumoState_FatalException);
            break;
        }
        case PneumoState_14: {
            int cylinderSignals[] = {NC, NC, NC, 1, 1, 0, 0, NC };
            pneumoStateBody(engine, cylinderSignals, PneumoState_15, PneumoState_FatalException);
            break;
        }
        case PneumoState_15: {
            int cylinderSignals[] = {1, 1, NC, NC, 0, NC, 1, 0 };
            pneumoStateBody(engine, cylinderSignals, PneumoState_16, PneumoState_FatalException);
            break;
        }
        case PneumoState_16: {
            int cylinderSignals[] = {0, NC, 0, 0, 1, 1, 0, 1 };
            pneumoStateBody(engine, cylinderSignals, PneumoState_17, PneumoState_FatalException);
            break;
        }
        case PneumoState_17: {
            int cylinderSignals[] = {NC, 0, 1, 1, 0, 0, NC, NC };
            pneumoStateBody(engine, cylinderSignals, PneumoState_0, PneumoState_FatalException);
            break;
        }
        case PneumoState_FatalException: {
            ret = false;
            break;
        }
    }
    engine->timeout++;
    return ret;
}