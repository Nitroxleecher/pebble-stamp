
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

void updateTimeTableModeDisplay()
{
    uint line;
    uint stampIndex;
    uint textIndex;
    
    for (stampIndex = 0, textIndex = 0; stampIndex < MAXNUM_TIMESPANS * 2; stampIndex++, textIndex++)
    {
        if (stampIndex < numStamps)
        {
            printTime(stampText[textIndex], stamp[stampIndex]);
        }
        else
        {
            memset(stampText[textIndex], 0, STAMP_TEXT_SIZE);
        }
    }
    
    for (line = 0; line < MAXNUM_TIMESPANS; line++)
    {
        setTableTimeText(line * 2, stampText[line * 2]);
        
        if (line * 2 + 1 < numStamps)
        {
            setTableSpacerText(line, stringLine);
        }
        else
        {
            setTableSpacerText(line, stringEmpty);
        }
        
        setTableTimeText(line * 2 + 1, stampText[line * 2 + 1]);
    }
    
    clearHighlight();
}

void updateTimeTableModeRemove()
{
    uint line;
    uint stampIndex;
    uint textIndex;
    
    for (stampIndex = 0, textIndex = 0; stampIndex < MAXNUM_TIMESPANS * 2; stampIndex++, textIndex++)
    {
        if (stampIndex < numStamps)
        {
            printTime(stampText[textIndex], stamp[stampIndex]);
        }
        else
        {
            memset(stampText[textIndex], 0, STAMP_TEXT_SIZE);
        }
    }
    
    for (line = 0; line < MAXNUM_TIMESPANS; line++)
    {
        setTableTimeText(line * 2, stampText[line * 2]);
        
        if (line * 2 + 1 < numStamps)
        {
            setTableSpacerText(line, stringLine);
        }
        else
        {
            setTableSpacerText(line, stringEmpty);
        }
        
        setTableTimeText(line * 2 + 1, stampText[line * 2 + 1]);
    }
    
    higlightTableTimeLayer(editPos);
}

void updateTimeTableModeAdd()
{
    uint line;
    uint stampIndex;
    uint textIndex;
    
    for (stampIndex = 0, textIndex = 0; stampIndex < MAXNUM_TIMESPANS * 2; stampIndex++, textIndex++)
    {
        if (stampIndex == editPos)
        {
            memset(stampText[textIndex], 0, STAMP_TEXT_SIZE);
            textIndex++;
        }
        
        if (stampIndex < numStamps)
        {
            printTime(stampText[textIndex], stamp[stampIndex]);
        }
        else if (textIndex < MAXNUM_TIMESPANS * 2)
        {
            memset(stampText[textIndex], 0, STAMP_TEXT_SIZE);
        }
    }
    
    for (line = 0; line < MAXNUM_TIMESPANS; line++)
    {
        setTableTimeText(line * 2, stampText[line * 2]);
        
        if (line * 2 + 1 < numStamps + 1)
        {
            setTableSpacerText(line, stringLine);
        }
        else
        {
            setTableSpacerText(line, stringEmpty);
        }
        
        setTableTimeText(line * 2 + 1, stampText[line * 2 + 1]);
    }
    
    higlightTableTimeLayer(editPos);
}

void application_update_view()
{
    switch(appMode)
    {
        case APPMODE_DISPLAY: updateTimeTableModeDisplay(); break;
        case APPMODE_ADD: updateTimeTableModeAdd(); break;
        case APPMODE_REMOVE: updateTimeTableModeRemove(); break;
    }
}

void application_cycle_click_handler(ClickRecognizerRef recognizer, void *context)
{
    switch(appMode)
    {
        case APPMODE_DISPLAY:
            if (numStamps < MAXNUM_TIMESPANS * 2)
            {
                application_switch_mode(APPMODE_ADD);
            }
            else
            {
                application_switch_mode(APPMODE_REMOVE);
            }
            break;
        case APPMODE_ADD:
            application_switch_mode(APPMODE_DISPLAY);
            
            if (numStamps > 0)
                application_switch_mode(APPMODE_REMOVE);
            else
                application_switch_mode(APPMODE_DISPLAY);
            
            break;
        case APPMODE_REMOVE: application_switch_mode(APPMODE_DISPLAY); break;
    }
}

void application_makeStamp_click_handler(ClickRecognizerRef recognizer, void *context)
{
    time_t currentTimestamp;
    struct tm* currentTime;
    
    if (numStamps >= MAXNUM_TIMESPANS * 2) return;
    
    time(&currentTimestamp);
    currentTime = localtime(&currentTimestamp);
    
    stamp[numStamps].hours = (uint8_t) currentTime->tm_hour;
    stamp[numStamps].minutes = (uint8_t) currentTime->tm_min;
    
    numStamps++;
    
    application_update_view();
}

void application_selectPrev_click_handler(ClickRecognizerRef recognizer, void *context)
{
    if (editPos > 0) editPos--;
    application_update_view();
}

void application_selectNext_click_handler(ClickRecognizerRef recognizer, void *context)
{
    if (editPos < numStamps) editPos++;
    application_update_view();
}

static void action_bar_click_config_provider_mode_display(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, application_cycle_click_handler);
    //window_single_click_subscribe(BUTTON_ID_UP, );
    window_single_click_subscribe(BUTTON_ID_DOWN, application_makeStamp_click_handler);
}

static void action_bar_click_config_provider_mode_add(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, application_cycle_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, application_selectPrev_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, application_selectNext_click_handler);
}

static void action_bar_click_config_provider_mode_remove(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, application_cycle_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, application_selectPrev_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, application_selectNext_click_handler);
}


void application_switch_mode(TAppMode mode)
{
    switch(mode)
    {
        case APPMODE_DISPLAY:
            clearActionBarIcon(BUTTON_ID_UP);
            setActionBarIcon(BUTTON_ID_SELECT, editIcon);
            setActionBarIcon(BUTTON_ID_DOWN, clockIcon);
            setActionBarClickProvider(action_bar_click_config_provider_mode_display);
        break;
        
        case APPMODE_ADD:
            if (numStamps >= MAXNUM_TIMESPANS * 2)
                return;
            clearActionBarIcon(BUTTON_ID_UP);
            setActionBarIcon(BUTTON_ID_SELECT, editIcon);
            clearActionBarIcon(BUTTON_ID_DOWN);
            setActionBarClickProvider(action_bar_click_config_provider_mode_add);
        break;
        
        case APPMODE_REMOVE:
            if (numStamps == 0)
                return;
            clearActionBarIcon(BUTTON_ID_UP);
            setActionBarIcon(BUTTON_ID_SELECT, editIcon);
            clearActionBarIcon(BUTTON_ID_DOWN);
            setActionBarClickProvider(action_bar_click_config_provider_mode_remove);
        break;
    }
    
    appMode = mode;
    application_update_view();
}

void application_init()
{
    APP_LOG(APP_LOG_LEVEL_DEBUG, "appinit");
    /*
    time_t savedTimestamp;
    struct tm* savedTime;
    
    time_t currentTimestamp;
    struct tm* currentTime;
    */
    
    appMode = APPMODE_DISPLAY;
    editPos = 0;
    editStamp.hours = 0;
    editStamp.minutes = 0;
    
    numStamps = 0;
    
    /*
    time(&currentTimestamp);
    currentTime = localtime(&currentTimestamp);
    */
    memset(stamp, 0, sizeof(TDayTime) * MAXNUM_TIMESPANS * 2);
    /*
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
    */
    
    application_switch_mode(APPMODE_DISPLAY);
    
    //application_update_view();
}

void application_deinit()
{

}