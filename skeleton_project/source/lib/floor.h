#include "driver/elevio.h"

int floorId = elevio_floorSensor();

void setLight(int floorId)
{
    elevio_floorIndicator(floorId);
}