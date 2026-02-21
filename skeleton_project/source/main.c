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
    
    int sensorFloor = floor_getSensor();
    if (sensorFloor != 0)
    {
        printf("=== Moving to first floor ===\n");

        while (floor_getSensor() != 0)
        {
            elevio_motorDirection(DIRN_DOWN);
        }

        elevio_motorDirection(DIRN_STOP);  

        printf("=== Ready for orders ===\n");
    }

    int lastKnownFloor = 0;
    bool stopLatched = false;
    changeState(DOOR_CLOSED);
    floor_startPollingThread();
    orderButtons_startPollingThread();

    while (1)
    {
        int stopPressed = elevio_stopButton();
        elevio_stopLamp(stopPressed);

        int sensorFloor = floor_getSensor();

        if (sensorFloor >= 0) // Between floors is -1
        {
            lastKnownFloor = sensorFloor;
            floor_setIndicator(sensorFloor);
        }
        else
        {
            lastKnownFloor = floor_getLastKnown();
        }

        door_update();

        // FAT S4/S5/S6/D3: immediate stop, clear orders, ignore new orders, open door at floor.
        if (stopPressed)
        {
            changeState(STOP);
            elevio_motorDirection(DIRN_STOP);

            if (!stopLatched)
            {
                queue_clearAllOrders();
                door_open();
                stopLatched = true;
            }

            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }
        stopLatched = false;

        if (state_p->state == STOP)
        {
            elevio_motorDirection(DIRN_STOP);
            if (sensorFloor < 0)
            {
                changeState(DOOR_CLOSED);
            }
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        if (state_p->state == DOOR_OPEN)
        {
            elevio_motorDirection(DIRN_STOP);
            queue_updateCurrentOrder();
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        queue_updateCurrentOrder();

        if (!queue_hasCurrentOrder())
        {
            elevio_motorDirection(DIRN_STOP);
            changeState(DOOR_CLOSED);
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        MotorDirection dir = queue_getDirectionToCurrentOrder(lastKnownFloor);

        if ((lastKnownFloor == 0 && dir == DIRN_DOWN) ||
            (lastKnownFloor == N_FLOORS - 1 && dir == DIRN_UP))
        {
            elevio_motorDirection(DIRN_STOP);
            orderQueue.currentOrder = -1;
            changeState(DOOR_CLOSED);
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        if (sensorFloor >= 0 &&
            queue_hasOrderAtFloorInDirection(sensorFloor, dir))
        {
            elevio_motorDirection(DIRN_STOP);
            queue_completeOrder(sensorFloor);
            door_open();
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        if (dir == DIRN_STOP)
        {
            elevio_motorDirection(DIRN_STOP);
            changeState(DOOR_CLOSED);
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
