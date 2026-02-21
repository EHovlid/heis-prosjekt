#pragma once

#include <stdbool.h>
#include "orderButton.h"

struct Queue
{
    int currentOrder;
    struct OrderButton *buttons;
    int numButtons;
};

extern struct Queue orderQueue;

void queue_updateCurrentOrder(void);
void queue_completeOrder(int floorId);
void queue_clearAllOrders(void);
bool queue_hasCurrentOrder(void);
MotorDirection queue_getDirectionToCurrentOrder(int currentFloor);
