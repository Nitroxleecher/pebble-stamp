/*
 * ui.h
 * contains all UI components
 */
#ifndef UI_H_
#define UI_H_

#include <pebble.h>
#include "configuration.h"

// WINDOWS ---------------------------------------------------------------------

// MAIN WINDOW - shows stamp table. modes: show / add / remove
extern Window *mainwindow;

void mainwindow_load(Window *mainwindow);
void mainwindow_unload(Window *mainwindow);
void mainwindow_init(void);
void mainwindow_deinit(void);

// Textlayers:
extern TextLayer *timeTableLayers[MAXNUM_TIMESPANS];

// RESOURCES -------------------------------------------------------------------

extern GBitmap* clockIcon;
extern GBitmap* settingsIcon;
extern GBitmap* editIcon;
extern GBitmap* backImage;

void resources_init();
void resources_deinit();

void setTableTimeText(int index, const char* text);
void setTableSpacerText(int index, const char* text);

void ui_init();
void ui_deinit();

#endif /* UI_H_ */