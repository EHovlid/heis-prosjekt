#include "state_machine.h"

struct StateMachine sateMachine = {DOOR_CLOSED};
struct StateMachine *state_p = &sateMachine;

bool changeState(elev_state new_state)
{
    elev_state prev_state = state_p->state;
    if (prev_state == new_state)
    {
        return true;
    }

    switch (new_state)
    {
    case MOVING:
        if (elevio_stopButton() == 1)
        {
            return false;
        }
        if (prev_state != DOOR_CLOSED)
        {
            return false;
        }
        state_p->state = MOVING;
        return true;

    case STOP:
        state_p->state = STOP;
        return true;

    case DOOR_OPEN:
        if (prev_state != DOOR_CLOSED && prev_state != STOP)
        {
            return false;
        }
        if (elevio_floorSensor() < 0)
        {
            return false;
        }
        state_p->state = DOOR_OPEN;
        return true;

    case DOOR_CLOSED:
        if (prev_state == DOOR_OPEN && (elevio_obstruction() == 1 || elevio_stopButton() == 1))
        {
            return false;
        }
        state_p->state = DOOR_CLOSED;
        return true;

    default:
        return false;
    }
}
