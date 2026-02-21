#pragma once

#include <stdbool.h>
#include "../driver/elevio.h"

struct OrderButton
{
    int floorId;
    bool isActive;
    ButtonType type;
};

extern struct OrderButton buttons[10];

void orderButtons_poll(void);
void setLight(int floorId, ButtonType button, bool enable);