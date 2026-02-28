#include "orderButton.h"
#include <time.h>

struct OrderButton buttons[ORDER_BUTTON_COUNT] = {
    {0, false, false, BUTTON_HALL_UP},
    {0, false, false, BUTTON_CAB},
    {1, false, false, BUTTON_HALL_UP},
    {1, false, false, BUTTON_HALL_DOWN},
    {1, false, false, BUTTON_CAB},
    {2, false, false, BUTTON_HALL_UP},
    {2, false, false, BUTTON_HALL_DOWN},
    {2, false, false, BUTTON_CAB},
    {3, false, false, BUTTON_HALL_DOWN},
    {3, false, false, BUTTON_CAB},
};

static pthread_mutex_t orderButtonsMtx = PTHREAD_MUTEX_INITIALIZER;
static bool pollingThreadStarted = false;

void orderButtons_lock(void)
{
    pthread_mutex_lock(&orderButtonsMtx);
}

void orderButtons_unlock(void)
{
    pthread_mutex_unlock(&orderButtonsMtx);
}

void orderButtons_setLight(int floorId, ButtonType button, bool enable)
{
    for (int i = 0; i < ORDER_BUTTON_COUNT; i++)
    {
        if (buttons[i].floorId == floorId && buttons[i].type == button)
        {
            // Do not change lamp state unless necessary
            if (buttons[i].isLightOn != enable)
            {
                elevio_buttonLamp(floorId, button, (int)enable);
                buttons[i].isLightOn = enable;
            }
            return;
        }
    }
}

// Check for active order and set light
void orderButtons_poll(void)
{
    int stopPressed = elevio_stopButton();

    // FAT H4 Poll all buttons for presses
    for (int i = 0; i < ORDER_BUTTON_COUNT; i++)
    {
        bool pressed = elevio_callButton(buttons[i].floorId, buttons[i].type);
        bool isActive;

        orderButtons_lock();
        if (!stopPressed && pressed)
        {
            buttons[i].isActive = true;
        }
        isActive = buttons[i].isActive;
        orderButtons_unlock();

        // FAT L3/L4 Light active buttons
        orderButtons_setLight(buttons[i].floorId, buttons[i].type, isActive);
    }
}

static void *orderButtons_pollingThread(void *arg)
{
    (void)arg;

    while (1)
    {
        orderButtons_poll();
        nanosleep(&(struct timespec){0, 20 * 1000 * 1000}, NULL);
    }

    return NULL;
}

void orderButtons_startPollingThread(void)
{
    if (pollingThreadStarted)
    {
        return;
    }

    pthread_t thread;
    if (pthread_create(&thread, NULL, orderButtons_pollingThread, NULL) == 0)
    {
        pthread_detach(thread);
        pollingThreadStarted = true;
    }
}
