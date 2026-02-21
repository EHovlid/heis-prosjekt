#pragma once

#include <stdbool.h>
#include "../driver/elevio.h"

typedef enum
{
    MOVING,
    STOP,
    DOOR_OPEN,
    DOOR_CLOSED,
} elev_state;

struct StateMachine
{
    elev_state state;
};

extern struct StateMachine sateMachine;
extern struct StateMachine *state_p;

bool changeState(elev_state new_state);