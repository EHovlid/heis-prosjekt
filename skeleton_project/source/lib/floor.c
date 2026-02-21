#include "floor.h"

int floor_getSensor(void)
{
    return elevio_floorSensor();
}

void floor_setIndicator(int floor)
{
    if (floor >= 0)
    {
        elevio_floorIndicator(floor);
    }
}
