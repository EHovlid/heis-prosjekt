#include <elevio.h>
#include <cstddef>

struct OrderButton
{
    int floorId;
    bool isActive;
    ButtonType type;
};

struct OrderButton buttons[10] = {
    {1, false, BUTTON_HALL_UP},
    {1, false, BUTTON_CAB},
    {2, false, BUTTON_HALL_UP},
    {2, false, BUTTON_HALL_DOWN},
    {2, false, BUTTON_CAB},
    {3, false, BUTTON_HALL_UP},
    {3, false, BUTTON_HALL_DOWN},
    {3, false, BUTTON_CAB},
    {4, false, BUTTON_HALL_DOWN},
    {4, false, BUTTON_CAB},
};

void setLight(int floorId, ButtonType button, bool enable)
{
    elevio_buttonLamp(floorId, button, enable);
}