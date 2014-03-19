
#ifndef APP_TYPES_H_
#define APP_TYPES_H_

#include <pebble.h>
#include "configuration.h"

typedef struct {
    uint8_t hours;
    uint8_t minutes;
} TDayTime;

typedef enum {
    APPMODE_DISPLAY,
    APPMODE_ADD,
    APPMODE_REMOVE
} TAppMode;

extern TDayTime stamp[MAXNUM_TIMESPANS * 2];
extern uint numStamps;

extern TDayTime editStamp;
extern TAppMode appMode;
extern uint editPos;



#define KEY_SAVEDATE 0x01UL
#define KEY_STAMPS 0x02UL
#define KEY_STAMPINDEX 0x03UL
#define KEY_TIMETOWORK 0x04UL


void printTime(char* text, TDayTime time);

void application_update_view();
void application_init();
void application_deinit();

#endif /* APP_TYPES_H_ */