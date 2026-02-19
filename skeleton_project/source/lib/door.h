#include <elevio.h>
struct Door
{
    bool isOpen;
    bool obstruction;
    bool stop;
};

void openDoor(bool open)
{
    elevio_doorOpenLamp((int)open);
}
