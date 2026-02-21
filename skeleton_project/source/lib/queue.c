#include "queue.h"

struct Queue orderQueue = {-1, buttons, 10};

bool queue_hasCurrentOrder(void)
{
    return orderQueue.currentOrder >= 0;
}

MotorDirection queue_getDirectionToCurrentOrder(int currentFloor)
{
    if (!queue_hasCurrentOrder())
    {
        return DIRN_STOP;
    }

    if (orderQueue.currentOrder > currentFloor)
    {
        return DIRN_UP;
    }
    if (orderQueue.currentOrder < currentFloor)
    {
        return DIRN_DOWN;
    }
    return DIRN_STOP;
}

void queue_updateCurrentOrder(void)
{
    // Return if already fulfilling order
    if (queue_hasCurrentOrder())
    {
        return;
    }

    // Set first active orderButton to current order
    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        if (orderQueue.buttons[i].isActive)
        {
            orderQueue.currentOrder = orderQueue.buttons[i].floorId;
            return;
        }
    }
}

void queue_completeOrder(int floorId)
{
    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        // All buttons on stopped floor should be cleared
        if (orderQueue.buttons[i].floorId == floorId)
        {
            orderQueue.buttons[i].isActive = false;
            orderButtons_setLight(orderQueue.buttons[i].floorId, orderQueue.buttons[i].type, false);
        }
    }

    if (orderQueue.currentOrder == floorId)
    {
        orderQueue.currentOrder = -1;
    }
}

void queue_clearAllOrders(void)
{
    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        orderQueue.buttons[i].isActive = false;
        orderButtons_setLight(orderQueue.buttons[i].floorId, orderQueue.buttons[i].type, false);
    }

    orderQueue.currentOrder = -1;
}
