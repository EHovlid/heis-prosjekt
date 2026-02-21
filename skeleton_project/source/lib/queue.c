#include "queue.h"

struct Queue orderQueue = {-1, buttons, 10};

bool queue_hasActiveOrder(void)
{
    return orderQueue.activeOrder >= 0;
}

MotorDirection queue_getDirectionToActiveOrder(int currentFloor)
{
    if (!queue_hasActiveOrder())
    {
        return DIRN_STOP;
    }

    if (orderQueue.activeOrder > currentFloor)
    {
        return DIRN_UP;
    }
    if (orderQueue.activeOrder < currentFloor)
    {
        return DIRN_DOWN;
    }
    return DIRN_STOP;
}

void queue_updateActiveOrder(void)
{
    // Return if already fulfilling order
    if (queue_hasActiveOrder())
    {
        return;
    }

    // Set first active order to current order
    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        if (orderQueue.buttons[i].isActive)
        {
            orderQueue.activeOrder = orderQueue.buttons[i].floorId;
            return;
        }
    }
}

void queue_completeOrder(int floorId)
{
    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        if (orderQueue.buttons[i].floorId == floorId)
        {
            orderQueue.buttons[i].isActive = false;
            setLight(orderQueue.buttons[i].floorId, orderQueue.buttons[i].type, false);
        }
    }

    if (orderQueue.activeOrder == floorId)
    {
        orderQueue.activeOrder = -1;
    }
}
