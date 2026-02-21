#include <stdio.h>
#include <time.h>

#include "driver/elevio.h"
#include "lib/floor.h"
#include "lib/orderButton.h"
#include "lib/queue.h"
#include "lib/state_machine.h"

int main(void)
{
    elevio_init();

    printf("=== Example Program ===\n");
    printf("Press the stop button on the elevator panel to exit\n");

    int lastKnownFloor = 0;

    while (1)
    {
        int sensorFloor = floor_getSensor();
        if (sensorFloor >= 0)
        {
            lastKnownFloor = sensorFloor;
            floor_setIndicator(sensorFloor);
        }

        // Emergency stop
        if (elevio_stopButton())
        {
            elevio_motorDirection(DIRN_STOP);
            changeState(STOP);
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        // Tries to exit emergency stop and go back to idle (DOOR_CLOSED)
        if (state_p->state == STOP)
        {
            if (!changeState(DOOR_CLOSED))
            {
                elevio_motorDirection(DIRN_STOP);
                nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
                continue;
            }
        }

        orderButtons_poll();
        queue_updateActiveOrder();

        // Idle if no active order
        if (!queue_hasActiveOrder())
        {
            elevio_motorDirection(DIRN_STOP);
            changeState(DOOR_CLOSED);
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        int targetFloor = orderQueue.activeOrder;

        // Open door if on correct floor
        if (sensorFloor == targetFloor)
        {
            elevio_motorDirection(DIRN_STOP);

            if (changeState(DOOR_OPEN))
            {
                elevio_doorOpenLamp(1);
                queue_completeOrder(targetFloor);
                nanosleep(&(struct timespec){3, 0}, NULL);
                elevio_doorOpenLamp(0);
                changeState(DOOR_CLOSED);
            }

            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        MotorDirection dir = queue_getDirectionToActiveOrder(lastKnownFloor);

        // Stop elevator if atempting to move outside valid range
        if ((lastKnownFloor == 0 && dir == DIRN_DOWN) ||
            (lastKnownFloor == N_FLOORS - 1 && dir == DIRN_UP))
        {
            elevio_motorDirection(DIRN_STOP);
            changeState(DOOR_CLOSED);
            orderQueue.activeOrder = -1;
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        if (changeState(MOVING))
        {
            elevio_motorDirection(dir);
        }
        else
        {
            elevio_motorDirection(DIRN_STOP);
            changeState(DOOR_CLOSED);
        }

        nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
    }
}
