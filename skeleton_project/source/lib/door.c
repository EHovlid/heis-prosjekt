#include "door.h"

#include <time.h>

#include "../driver/elevio.h"
#include "state_machine.h"

bool door_open(void)
{
    if (!changeState(DOOR_OPEN))
    {
        return false;
    }

    elevio_doorOpenLamp(1);
    nanosleep(&(struct timespec){3, 0}, NULL);

    changeState(DOOR_CLOSED);
    elevio_doorOpenLamp(0);
    return true;
}
