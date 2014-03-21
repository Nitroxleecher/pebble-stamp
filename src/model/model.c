#include "model.h"
#include <string.h>
#include <time.h>

TSettings settings;
TAppData appData;

void model_init()
{
    memset(appData.stamp, 0, sizeof(appData.stamp));
    appData.stampCount = 0;
}

void model_deinit()
{

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