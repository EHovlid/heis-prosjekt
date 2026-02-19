#include "orderButton.h"

struct Queue
{
    int activeOrder;
    struct OrderButton *buttons;
};

struct Queue orderQueue = {-1, buttons};

void completeOrder(int floorId)
{
    size_t num_buttons = sizeof(orderQueue.buttons) / sizeof(orderQueue.buttons[0]);

    for (int i = 0; i < num_buttons; i++)
    {
        if (orderQueue.buttons[i].floorId == floorId)
        {
            orderQueue.buttons[i].isActive = false;
        }
    }
}