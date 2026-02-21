#pragma once

#include <stdbool.h>
#include "orderButton.h"

struct Queue
{
    int activeOrder;
    struct OrderButton *buttons;
    int numButtons;
};

extern struct Queue orderQueue;

void queue_updateActiveOrder(void);
void queue_completeOrder(int floorId);
bool queue_hasActiveOrder(void);
MotorDirection queue_getDirectionToActiveOrder(int currentFloor);
