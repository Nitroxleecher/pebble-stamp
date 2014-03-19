
#include "configuration.h"
#include "application.h"
#include <pebble.h>
#include <time.h>
#include "ui.h"

TDayTime stamp[MAXNUM_TIMESPANS * 2];

#define STAMP_TEXT_SIZE 6
char stampText[MAXNUM_TIMESPANS * 2][STAMP_TEXT_SIZE];

const char* stringEmpty = "";
const char* stringLine = "-";

uint numStamps;

TDayTime editStamp;
TAppMode appMode;
uint editPos;

void printTime(char* text, TDayTime time)
{
    char highHour = time.hours / 10 + '0';
    char lowHour = time.hours % 10 + '0';
    char highMin = time.minutes / 10 + '0';
    char lowMin = time.minutes % 10 + '0';
 
    text[0] = highHour;
    text[1] = lowHour;
    text[2] = ':';
    text[3] = highMin;
    text[4] = lowMin;
}

void updateTimeTable()
{
    uint line;
    uint offset;
    
    for (line = 0; line < MAXNUM_TIMESPANS; line++)
    {
        if (APPMODE_ADD && line * 2 == editPos) offset = 1;
    
        if (line * 2 < numStamps + offset)
        {
            printTime(stampText[line * 2], stamp[line * 2] - offset);
        }
        else
        {
            memset(stampText[line * 2], 0, STAMP_TEXT_SIZE);
        }
        setTableTimeText(line * 2, stampText[line * 2]);
        
        if (APPMODE_ADD && line * 2 + 1 == editPos) offset = 1;
        
        if (line * 2 + 1 < numStamps + offset)
        {
            printTime(stampText[line * 2 + 1], stamp[line * 2] - offset);
            setTableSpacerText(line, stringLine);
        }
        else
        {
            memset(stampText[line * 2 + 1], 0, STAMP_TEXT_SIZE);
            setTableSpacerText(line, stringEmpty);
        }
        
        setTableTimeText(line * 2 + 1, stampText[line * 2 + 1]);
    }
}

void application_update_view()
{
    updateTimeTable();
}

void application_switch_mode(TAppMode mode)
{
    if (mode == APPMODE_ADD && numStamps >= MAXNUM_TIMESPANS * 2)
    return;
    
    if (mode == APPMODE_REMOVE && numStamps == 0)
    return;
    
    appMode = mode;
    application_update_view();
}

void application_init()
{
    time_t savedTimestamp;
    struct tm* savedTime;
    
    time_t currentTimestamp;
    struct tm* currentTime;
    
    
    appMode = APPMODE_DISPLAY;
    editPos = 0;
    editStamp.hours = 0;
    editStamp.minutes = 0;
    
    numStamps = 0;
    
    time(&currentTimestamp);
    currentTime = localtime(&currentTimestamp);
    
    memset(stamp, 0, sizeof(TDayTime) * MAXNUM_TIMESPANS * 2);
    
    if (persist_exists(KEY_SAVEDATE) && persist_exists(KEY_STAMPS))
    {
        persist_read_data(KEY_SAVEDATE, (void*)&savedTimestamp, sizeof(savedTimestamp));
        
        savedTime = localtime(&savedTimestamp);
        
        if (savedTime->tm_year == currentTime->tm_year && savedTime->tm_yday == currentTime->tm_yday)
        {
            persist_read_data(KEY_STAMPS, (void*)stamp, sizeof(TDayTime) * MAXNUM_TIMESPANS * 2);
            persist_read_data(KEY_STAMPINDEX, (void*)&numStamps, sizeof(numStamps));
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
    
    application_update_view();
}

void application_deinit()
{

}