#include "queue.h"

struct Queue orderQueue = {-1, buttons, 10};

bool queue_hasCurrentOrder(void)
{
    return orderQueue.currentOrder >= 0;
}

bool queue_hasOrderAtFloorInDirection(int floorId, MotorDirection dir)
{
    orderButtons_lock();
    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        // No active orders
        struct OrderButton button = orderQueue.buttons[i];
        if (!button.isActive || button.floorId != floorId)
        {
            continue;
        }

        if (button.type == BUTTON_CAB)
        {
            orderButtons_unlock();
            return true;
        }

        // FAT H3 Ingore orders in wrong direction
        if (dir == DIRN_UP && button.type == BUTTON_HALL_UP)
        {
            orderButtons_unlock();
            return true;
        }
        if (dir == DIRN_DOWN && button.type == BUTTON_HALL_DOWN)
        {
            orderButtons_unlock();
            return true;
        }
        if (dir == DIRN_STOP)
        {
            orderButtons_unlock();
            return true;
        }
    }

    orderButtons_unlock();
    return false;
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

    // FAT H5 Set first active orderButton to current order
    orderButtons_lock();
    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        if (orderQueue.buttons[i].isActive)
        {
            orderQueue.currentOrder = orderQueue.buttons[i].floorId;
            orderButtons_unlock();
            return;
        }
    }
    orderButtons_unlock();
}

void queue_completeOrder(int floorId)
{
    bool clearMask[10] = {false};

    orderButtons_lock();
    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        // All buttons on stopped floor should be cleared
        if (orderQueue.buttons[i].floorId == floorId)
        {
            orderQueue.buttons[i].isActive = false;
            clearMask[i] = true;
        }
    }
    orderButtons_unlock();

    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        if (clearMask[i])
        {
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
    orderButtons_lock();
    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        orderQueue.buttons[i].isActive = false;
    }
    orderButtons_unlock();

    for (int i = 0; i < orderQueue.numButtons; i++)
    {
        orderButtons_setLight(orderQueue.buttons[i].floorId, orderQueue.buttons[i].type, false);
    }

    orderQueue.currentOrder = -1;
}
