#pragma once

#include <stdbool.h>
#include <pthread.h>
#include "../driver/elevio.h"

struct OrderButton
{
    int floorId;
    bool isActive;
    bool isLightOn;
    ButtonType type;
};


// Compile time const for number of buttons
enum
{
    ORDER_BUTTON_COUNT = N_FLOORS * N_BUTTONS - 2
};

extern struct OrderButton buttons[ORDER_BUTTON_COUNT];

void orderButtons_poll(void);
void orderButtons_setLight(int floorId, ButtonType button, bool enable);
void orderButtons_startPollingThread(void);
void orderButtons_lock(void);
void orderButtons_unlock(void);
