#pragma once

#include <pebble.h>

#define MAX_NUM_TIME_PERIODS 6


#define MAX_NUM_STAMPS (MAX_NUM_TIME_PERIODS * 2)

typedef struct {
    uint8_t hours;
    uint8_t minutes;
} TDayTime;

typedef struct {
    TDayTime timeToWork;
} TSettings;

typedef struct {
    TDayTime stamp[MAX_NUM_STAMPS];
    unsigned int stampCount;
} TAppData;

extern TSettings settings;
extern TAppData appData;

void model_init();
void model_deinit();
void model_make_stamp();