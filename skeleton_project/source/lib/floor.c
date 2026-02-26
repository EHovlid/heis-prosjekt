#include "floor.h"

#include <stdbool.h>
#include <pthread.h>
#include <time.h>

static pthread_mutex_t floorMtx = PTHREAD_MUTEX_INITIALIZER;
static bool floorThreadStarted = false;
static int cachedSensorFloor = -1;
static int cachedLastKnownFloor = 0;

static void floor_cacheUpdate(int sensorFloor)
{
    pthread_mutex_lock(&floorMtx);
    cachedSensorFloor = sensorFloor;
    if (sensorFloor >= 0)
    {
        cachedLastKnownFloor = sensorFloor;
    }
    pthread_mutex_unlock(&floorMtx);
}

static void *floor_pollingThread(void *arg)
{
    (void)arg;

    while (1)
    {
        floor_cacheUpdate(elevio_floorSensor());
        nanosleep(&(struct timespec){0, 10 * 1000 * 1000}, NULL);
    }

    return NULL;
}

void floor_startPollingThread(void)
{
    if (floorThreadStarted)
    {
        return;
    }

    floor_cacheUpdate(elevio_floorSensor());

    pthread_t thread;
    if (pthread_create(&thread, NULL, floor_pollingThread, NULL) == 0)
    {
        pthread_detach(thread);
        floorThreadStarted = true;
    }
}

int floor_getSensor(void)
{
    if (!floorThreadStarted)
    {
        int sensor = elevio_floorSensor();
        floor_cacheUpdate(sensor);
        return sensor;
    }

    pthread_mutex_lock(&floorMtx);
    int sensor = cachedSensorFloor;
    pthread_mutex_unlock(&floorMtx);
    return sensor;
}

void floor_setIndicator(int floor)
{
    // FAT L4 Should light for last known floor
    if (floor >= 0)
    {
        elevio_floorIndicator(floor);
    }
}
