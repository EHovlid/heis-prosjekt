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
    int buttonPollTick = 0;
    changeState(DOOR_CLOSED);

    while (1)
    {
        
        printf("state=%d\n", state_p->state);

        elevio_stopLamp(state_p->state == STOP ? 1 : 0);

        int sensorFloor = floor_getSensor();
        if (sensorFloor >= 0)
        {
            lastKnownFloor = sensorFloor;
            floor_setIndicator(sensorFloor);
        }

        // FAT S4/S5/S6/D3: immediate stop, clear orders, ignore new orders, open door at floor.
        int stopPressed = elevio_stopButton();
        if (stopPressed)
        {
            changeState(STOP); // Stop status alwaus succeeds
            elevio_stopLamp(1);
            queue_clearAllOrders();   
            elevio_motorDirection(DIRN_STOP);
            if (sensorFloor >= 0)
            {
                door_open();
            }
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        if (state_p->state == STOP)
        {
            // Clear stop
            if (!elevio_stopButton())
            {
                changeState(DOOR_CLOSED);
                elevio_stopLamp(0);
            }

            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        // Poll order buttons less frequently to avoid blocking floor updates.
        if (buttonPollTick == 0)
        {
            orderButtons_poll();
            queue_updateCurrentOrder();
            buttonPollTick = 4;
        }
        else
        {
            buttonPollTick--;
        }

        if (!queue_hasCurrentOrder())
        {
            elevio_motorDirection(DIRN_STOP);
            changeState(DOOR_CLOSED);
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        int targetFloor = orderQueue.currentOrder;
        if (sensorFloor == targetFloor)
        {
            elevio_motorDirection(DIRN_STOP);
            queue_completeOrder(targetFloor);
            door_open();
            nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
            continue;
        }

        MotorDirection dir = queue_getDirectionToCurrentOrder(lastKnownFloor);
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
