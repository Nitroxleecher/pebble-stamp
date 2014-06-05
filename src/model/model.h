#pragma once

#include <pebble.h>

#define log(...) app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

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
void model_insert_time(unsigned int editPos, TDayTime timeToInsert);
void model_remove_time(unsigned int editPos);
void model_remove_all(void);