#pragma once

#include "../driver/elevio.h"

void floor_startPollingThread(void);
int floor_getSensor(void);
void floor_setIndicator(int floor);
