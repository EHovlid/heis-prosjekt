#include <elevio.h>

enum elev_state
{
    MOVING,
    STOP,
    DOOR_OPEN,
    DOOR_CLOSED,
};

struct StateMachine
{
    enum elev_state state;
};

struct StateMachine sateMachine = {DOOR_CLOSED};
struct StateMachine *state_p = &sateMachine;

bool changeState(elev_state new_state)
{
    elev_state prev_state = state_p->state;
    switch (new_state)
    {
    case MOVING:
        if (elevio_stopButton() == 1 ||
            prev_state != DOOR_CLOSED)
        {
            return false;
        }

        state_p->state = new_state;
        return true;
        break;
    case STOP:
        state_p->state = new_state;
        return true;
    case DOOR_OPEN:
        if (elevio_floorSensor > 0)
        {
            state_p->state = new_state;
            return true;
        }
    case DOOR_CLOSED:
        if (prev_state == DOOR_OPEN &&
            (elevio_obstruction() == 1 || elevio_stopButton() == 1))
        {
            return false;
        }
        state_p->state = new_state;
        return true;
    default:
        return false;
    };
}
