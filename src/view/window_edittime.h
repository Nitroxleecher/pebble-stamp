#include <pebble.h>
#include "../model/model.h"

extern Window* window_edittime;

void window_init_edittime();
void window_deinit_edittime();

void window_edittime_configure(TDayTime preset,  void (*callbackApply)(TDayTime edited));