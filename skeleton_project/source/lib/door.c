#include "door.h"

#include <time.h>

#include "../driver/elevio.h"
#include "state_machine.h"

static bool doorTimerActive = false;
static long long closeAtMs = 0;

static long long now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000LL + (long long)ts.tv_nsec / 1000000LL;
}

bool door_open(void)
{
    if (state_p->state != DOOR_OPEN)
    {
        if (!changeState(DOOR_OPEN))
        {
            return false;
        }
        elevio_doorOpenLamp(1);
    }

    doorTimerActive = true;
    closeAtMs = now_ms() + 3000;
    return true;
}

void door_update(void)
{
    if (!doorTimerActive)
    {
        return;
    }

    if (state_p->state != DOOR_OPEN)
    {
        doorTimerActive = false;
        elevio_doorOpenLamp(0);
        return;
    }

    if (elevio_stopButton() || elevio_obstruction())
    {
        closeAtMs = now_ms() + 3000;
        return;
    }

    if (now_ms() >= closeAtMs)
    {
        if (changeState(DOOR_CLOSED))
        {
            elevio_doorOpenLamp(0);
            doorTimerActive = false;
        }
    }
}
