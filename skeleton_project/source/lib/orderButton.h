#include <elevio.h>
#include <cstddef>

struct OrderButton
{
    int floorId;
    bool isActive;
    ButtonType type;
};

void setLight(int floorId, ButtonType button, bool enable)
{
    elevio_buttonLamp(floorId, button, enable);
}