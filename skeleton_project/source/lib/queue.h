#include "orderButton.h"

struct OrderButton queue[10];

void completeOrder(int floorId)
{
    size_t num_buttons = sizeof(queue) / sizeof(queue[0]);

    for (int i = 0; i < num_buttons; i++)
    {
        if (queue[i].floorId == floorId)
        {
            queue[i].isActive = false;
        }
    }
}