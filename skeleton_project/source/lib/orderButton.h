#pragma once

#include <stdbool.h>
#include <pthread.h>
#include "../driver/elevio.h"

struct OrderButton
{
    int floorId;
    bool isActive;
    ButtonType type;
};

extern struct OrderButton buttons[10];

void orderButtons_poll(void);
void orderButtons_setLight(int floorId, ButtonType button, bool enable);
void orderButtons_startPollingThread(void);
void orderButtons_lock(void);
void orderButtons_unlock(void);
