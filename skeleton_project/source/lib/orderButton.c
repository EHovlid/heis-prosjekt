#include "orderButton.h"
#include <time.h>

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

static pthread_mutex_t orderButtonsMtx = PTHREAD_MUTEX_INITIALIZER;
static bool pollingThreadStarted = false;
static bool lampState[10] = {false};

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
    for (int i = 0; i < 10; i++)
    {
        if (buttons[i].floorId == floorId && buttons[i].type == button)
        {
            if (lampState[i] != enable)
            {
                elevio_buttonLamp(floorId, button, (int)enable);
                lampState[i] = enable;
            }
            return;
        }
    }
}

// Check for active order and set light
void orderButtons_poll(void)
{
    int stopPressed = elevio_stopButton();

    for (int i = 0; i < 10; i++)
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

        if (lampState[i] != isActive)
        {
            orderButtons_setLight(buttons[i].floorId, buttons[i].type, isActive);
        }
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
