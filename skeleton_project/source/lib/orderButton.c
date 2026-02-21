#include "orderButton.h"

struct OrderButton buttons[10] = {
    {0, false, BUTTON_HALL_UP},
    {0, false, BUTTON_CAB},
    {1, false, BUTTON_HALL_UP},
    {1, false, BUTTON_HALL_DOWN},
    {1, false, BUTTON_CAB},
    {2, false, BUTTON_HALL_UP},
    {2, false, BUTTON_HALL_DOWN},
    {2, false, BUTTON_CAB},
    {3, false, BUTTON_HALL_DOWN},
    {3, false, BUTTON_CAB},
};

void setLight(int floorId, ButtonType button, bool enable)
{
    elevio_buttonLamp(floorId, button, (int)enable);
}

// Check for active order and set light
void orderButtons_poll(void)
{
    for (int i = 0; i < 10; i++)
    {
        if (elevio_callButton(buttons[i].floorId, buttons[i].type))
        {
            buttons[i].isActive = true;
        }

        setLight(buttons[i].floorId, buttons[i].type, buttons[i].isActive);
    }
}