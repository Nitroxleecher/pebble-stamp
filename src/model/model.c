#include "model.h"
#include <string.h>
#include <time.h>

TSettings settings;
TAppData appData;

static time_t savedTimestamp;


static const uint32_t KEY_SAVEDATE = 0x01;
static const uint32_t KEY_STAMPS = 0x02;
static const uint32_t KEY_STAMPINDEX = 0x03;
static const uint32_t KEY_TIMETOWORK = 0x04;

void model_init()
{
    struct tm* savedTime;
    time_t currentTimestamp;
    struct tm* currentTime;
    
    memset(appData.stamp, 0, sizeof(appData.stamp));
    appData.stampCount = 0;

    time(&currentTimestamp);
    currentTime = localtime(&currentTimestamp);

    if (persist_exists(KEY_SAVEDATE) && persist_exists(KEY_STAMPS))
    {
        persist_read_data(KEY_SAVEDATE, (void*)&savedTimestamp, sizeof(savedTimestamp));

        savedTime = localtime(&savedTimestamp);

        if (savedTime->tm_year == currentTime->tm_year && savedTime->tm_yday == currentTime->tm_yday)
        {
            if (persist_get_size(KEY_STAMPS) == sizeof(TDayTime) * MAX_NUM_STAMPS)
            {
                persist_read_data(KEY_STAMPS, (void*)appData.stamp, sizeof(TDayTime) * MAX_NUM_STAMPS);
                persist_read_data(KEY_STAMPINDEX, (void*)&appData.stampCount, sizeof(appData.stampCount));
                if (appData.stampCount > MAX_NUM_STAMPS)
                {
                    appData.stampCount = MAX_NUM_STAMPS;
                }
            }
            else
            {
                savedTimestamp = currentTimestamp;
            }
        }
        else
        {
            savedTimestamp = currentTimestamp;
        }
    }
    else
    {
        savedTimestamp = currentTimestamp;
    }
}

void model_deinit()
{
    persist_write_data(KEY_SAVEDATE, (void*)&savedTimestamp, sizeof(savedTimestamp));
    persist_write_data(KEY_STAMPS, (void*)appData.stamp, sizeof(TDayTime) * MAX_NUM_STAMPS);
    persist_write_data(KEY_STAMPINDEX, (void*)&appData.stampCount, sizeof(appData.stampCount));
}

void model_make_stamp()
{
    time_t currentTimestamp;
    struct tm* currentTime;

    if (appData.stampCount >= MAX_NUM_STAMPS) return;

    //text_layer_set_text(text_layer, "Up");
    time(&currentTimestamp);
    currentTime = localtime(&currentTimestamp);

    appData.stamp[appData.stampCount].hours = (uint8_t) currentTime->tm_hour;
    appData.stamp[appData.stampCount].minutes = (uint8_t) currentTime->tm_min;

    appData.stampCount++;
}

void model_insert_time(unsigned int editPos, TDayTime timeToInsert)
{
    unsigned int i;
    appData.stampCount++;
    
    for (i = appData.stampCount; i > editPos; i--)
    {
        appData.stamp[i].hours = appData.stamp[i-1].hours;
        appData.stamp[i].minutes = appData.stamp[i-1].minutes;
    }
    
    appData.stamp[editPos].hours = timeToInsert.hours;
    appData.stamp[editPos].minutes = timeToInsert.minutes;
}

void model_remove_time(unsigned int editPos)
{
    unsigned int i;
    appData.stampCount--;
    
    for (i = editPos; i < appData.stampCount; i++)
    {
        appData.stamp[i].hours = appData.stamp[i+1].hours;
        appData.stamp[i].minutes = appData.stamp[i+1].minutes;
    }
    appData.stamp[appData.stampCount].hours = 0;
    appData.stamp[appData.stampCount].minutes = 0;
}