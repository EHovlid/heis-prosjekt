#include <stdio.h>
#include <time.h>

#include "driver/elevio.h"
#include "lib/door.h"
#include "lib/floor.h"
#include "lib/orderButton.h"
#include "lib/queue.h"
#include "lib/state_machine.h"

int main(void)
{
    elevio_init();

    printf("=== Elevator Controller ===\n");
    floor_startPollingThread();
    nanosleep(&(struct timespec){0, 100 * 1000 * 1000}, NULL); // Allow thread to start

    int sensorFloor = floor_getSensor();
    if (sensorFloor != 0)
    {
        printf("=== Moving to first floor ===\n");

        while (sensorFloor != 0)
        {
            elevio_motorDirection(DIRN_DOWN);
            sensorFloor = floor_getSensor();
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
        }

        elevio_motorDirection(DIRN_STOP);
    }
    printf("=== Ready for orders ===\n");

    int lastKnownFloor = 0;
    changeState(DOOR_CLOSED);
    orderButtons_startPollingThread();

    while (1)
    {
        nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);

        int stopPressed = elevio_stopButton();
        elevio_stopLamp(stopPressed);

        int sensorFloor = floor_getSensor();

        if (sensorFloor >= 0) // Between floors is -1
        {
            lastKnownFloor = sensorFloor;
        }

        floor_setIndicator(lastKnownFloor);

        door_update();

        // FAT S4/S5/S6/D3: immediate stop, clear orders, ignore new orders, open door at floor.
        if (stopPressed)
        {
            elevio_motorDirection(DIRN_STOP);
            queue_clearAllOrders();
            changeState(STOP);
            door_open();
            continue;
        }

        // Reset to idle (DOOR_CLOSED) when stopPressed is false
        if (state_p->state == STOP)
        {
            elevio_motorDirection(DIRN_STOP);
            changeState(DOOR_CLOSED);
            continue;
        }

        // Never run motor with door open regardelss of other states
        if (state_p->state == DOOR_OPEN)
        {
            elevio_motorDirection(DIRN_STOP);
            continue;
        }

        queue_updateCurrentOrder();

        if (!queue_hasCurrentOrder())
        {
            elevio_motorDirection(DIRN_STOP);
            changeState(DOOR_CLOSED);
            continue;
        }

        MotorDirection dir = queue_getDirectionToCurrentOrder(lastKnownFloor);

        // Stop moving if outside area
        if ((lastKnownFloor == 0 && dir == DIRN_DOWN) ||
            (lastKnownFloor == N_FLOORS - 1 && dir == DIRN_UP))
        {
            elevio_motorDirection(DIRN_STOP);
            orderQueue.currentOrder = -1;
            changeState(DOOR_CLOSED);
            continue;
        }

        // Check if should stop on floor
        if (sensorFloor >= 0 &&
            queue_hasOrderAtFloorInDirection(sensorFloor, dir))
        {
            changeState(DOOR_CLOSED);
            elevio_motorDirection(DIRN_STOP);
            queue_completeOrder(sensorFloor);
            door_open();
            continue;
        }

        if (dir == DIRN_STOP)
        {
            if (changeState(DOOR_CLOSED))
            {
                elevio_motorDirection(DIRN_STOP);
            }
            continue;
        }

        if (changeState(MOVING))
        {
            elevio_motorDirection(dir);
        }
        else
        {
            changeState(DOOR_CLOSED);
            elevio_motorDirection(DIRN_STOP);
        }
    }
}
