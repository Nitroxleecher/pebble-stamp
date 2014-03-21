#include "window_main.h"
#include "../model/model.h"
#include "window_editmenu.h"

#define EDIT_MENU_NUM_SECTIONS 1
//------------------------------------------------------------------------------
// PUBLIC VARIABLES

Window* window_main;

//------------------------------------------------------------------------------
// PRIVATE VARIABLES

static Layer* rootlayer;
static ActionBarLayer* action_bar;

static TextLayer* textLayerTime[MAX_NUM_STAMPS];
#define STAMP_TEXT_SIZE 6
static char stampText[MAX_NUM_STAMPS][STAMP_TEXT_SIZE];


static TextLayer* textLayerSpacer[MAX_NUM_TIME_PERIODS];

static TextLayer* textLayerTimeWorked;
static char timeWorkedText[] = "Total: 00:00";




static GBitmap* clockIcon;
static GBitmap* settingsIcon;
static GBitmap* editIcon;

static struct tm* currentTime;

//------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

static void window_load(Window *wnd);
static void window_unload(Window *wnd);
static void string_print_daytime(char* text, TDayTime time);
static void click_config_provider_main(void *context);

static void tickHandler(struct tm *tick_time, TimeUnits units_changed);

static void click_handler_up(ClickRecognizerRef recognizer, void *context);
static void click_handler_select(ClickRecognizerRef recognizer, void *context);
static void click_handler_down(ClickRecognizerRef recognizer, void *context);
static void click_handler_back(ClickRecognizerRef recognizer, void *context);
//==============================================================================
// INIT AND CLEANUP FUNCTIONS


//------------------------------------------------------------------------------
// INIT

void window_init_main()
{
    window_main = window_create();

    window_set_window_handlers(window_main, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });

    window_stack_push(window_main, true);
}

//------------------------------------------------------------------------------
// DEINIT

void window_deinit_main()
{
    window_destroy(window_main);
}

//------------------------------------------------------------------------------
// LOAD

#define FONT_SIZE 18

static void window_load(Window *wnd)
{
    time_t timestamp;
    GFont gothic18Font;
    int i;
    GRect bounds;
    
    time(&timestamp);
    currentTime = localtime(&timestamp);
    
    
    clockIcon = gbitmap_create_with_resource(RESOURCE_ID_CLOCK_ICON);
    settingsIcon = gbitmap_create_with_resource(RESOURCE_ID_SETTINGS_ICON);
    editIcon = gbitmap_create_with_resource(RESOURCE_ID_EDIT_ICON);

    // load system font
    gothic18Font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);


    rootlayer = window_get_root_layer(wnd);
    bounds = layer_get_frame(rootlayer);
    
    action_bar = action_bar_layer_create();

    action_bar_layer_add_to_window(action_bar, wnd);

    action_bar_layer_set_click_config_provider(action_bar, click_config_provider_main);

    action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, clockIcon);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, settingsIcon);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, editIcon);

    textLayerTimeWorked = text_layer_create(
        (GRect) {.origin = { 30, bounds.size.h - (FONT_SIZE) - 10 }, .size = { 94, FONT_SIZE } }
    );
    
    text_layer_set_font(textLayerTimeWorked, gothic18Font);
    
    
    
    // create and layout text layers
    for (i = 0; i < MAX_NUM_TIME_PERIODS; i++)
    {
        // left time
        textLayerTime[i*2] = text_layer_create(
            (GRect) {.origin = { 30, 0 + i * (FONT_SIZE) }, .size = { 37, FONT_SIZE } }
        );

        // space with " - " in the middle
        textLayerSpacer[i] = text_layer_create(
            (GRect) {.origin = { 67, 0 + i * (FONT_SIZE) }, .size = { 10, FONT_SIZE } }
        );

        // right time
        textLayerTime[i * 2 + 1] = text_layer_create(
            (GRect) {.origin = { 77, 0 + i * (FONT_SIZE) }, .size = { 37, FONT_SIZE } }
        );

        // set font for all layers
        text_layer_set_font(textLayerTime[i*2], gothic18Font);
        text_layer_set_font(textLayerTime[i*2 + 1], gothic18Font);
        text_layer_set_font(textLayerSpacer[i], gothic18Font);

        // set center alignment for spacer layer
        text_layer_set_text_alignment(textLayerSpacer[i], GTextAlignmentCenter);

        // add some random stuff
        text_layer_set_text(textLayerTime[i*2], "");
        text_layer_set_text(textLayerTime[i*2 + 1], "");
        text_layer_set_text(textLayerSpacer[i], "");

        // add all textlayers to root layer
        layer_add_child(rootlayer, text_layer_get_layer(textLayerTime[i*2]));
        layer_add_child(rootlayer, text_layer_get_layer(textLayerTime[i*2 + 1]));
        layer_add_child(rootlayer, text_layer_get_layer(textLayerSpacer[i]));
    }

    layer_add_child(rootlayer, text_layer_get_layer(textLayerTimeWorked));
    
    tick_timer_service_subscribe(MINUTE_UNIT, &tickHandler);
    
    window_update_view_main();
}

static void tickHandler(struct tm *tick_time, TimeUnits units_changed)
{
    currentTime = tick_time;
    window_update_view_main();
}

//------------------------------------------------------------------------------
// UNLOAD

static void window_unload(Window *wnd)
{
    int i;

    tick_timer_service_unsubscribe();
    
    action_bar_layer_destroy(action_bar);

    gbitmap_destroy(clockIcon);
    gbitmap_destroy(settingsIcon);
    gbitmap_destroy(editIcon);

    text_layer_destroy(textLayerTimeWorked);
    
    for (i = 0; i < MAX_NUM_TIME_PERIODS; i++)
    {
        text_layer_destroy(textLayerTime[i*2]);
        text_layer_destroy(textLayerSpacer[i]);
        text_layer_destroy(textLayerTime[i * 2 + 1]);

    }
}

//==============================================================================
// CLICK HANDLERS

static void click_config_provider_main(void *context)
{
    window_single_click_subscribe(BUTTON_ID_UP, click_handler_up);
    window_single_click_subscribe(BUTTON_ID_SELECT, click_handler_select);
    window_single_click_subscribe(BUTTON_ID_DOWN, click_handler_down);
    window_single_click_subscribe(BUTTON_ID_BACK, click_handler_back);
}

static void click_handler_up(ClickRecognizerRef recognizer, void *context)
{
    window_stack_push(window_editmenu, true);
}

static void click_handler_select(ClickRecognizerRef recognizer, void *context)
{
    
}

static void click_handler_down(ClickRecognizerRef recognizer, void *context)
{
    model_make_stamp();
    window_update_view_main();
}

static void click_handler_back(ClickRecognizerRef recognizer, void *context)
{
    window_stack_pop_all(true);
}

//==============================================================================
// APPLICATION FUNCTIONS


static void string_print_daytime(char* text, TDayTime time)
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



void window_update_view_main()
{
    uint line;
    uint stampIndex;
    int timeDelta = 0;
    TDayTime dayTimeWorked;
    
    for (stampIndex = 0; stampIndex < MAX_NUM_STAMPS; stampIndex++)
    {
        if (stampIndex < appData.stampCount)
        {
            string_print_daytime(stampText[stampIndex], appData.stamp[stampIndex]);
        }
        else
        {
            memset(stampText[stampIndex], 0, STAMP_TEXT_SIZE);
        }
    }

    for (line = 0; line < MAX_NUM_TIME_PERIODS; line++)
    {
        text_layer_set_text(textLayerTime[line * 2], stampText[line * 2]);

        if (line * 2 + 1 <  appData.stampCount)
        {
            text_layer_set_text(textLayerSpacer[line], "-");
        }
        else
        {
            text_layer_set_text(textLayerSpacer[line], "");
        }

         text_layer_set_text(textLayerTime[line * 2 + 1], stampText[line * 2 + 1]);
    }
    
    for (stampIndex = 0; stampIndex < appData.stampCount; stampIndex++)
    {
        if (stampIndex % 2 == 0)
        {
            timeDelta -= (appData.stamp[stampIndex].hours * 60);
            timeDelta -= appData.stamp[stampIndex].minutes;
        }
        else
        {
            timeDelta += (appData.stamp[stampIndex].hours * 60);
            timeDelta += appData.stamp[stampIndex].minutes;
        }
    }
    
    if (appData.stampCount > 0 && appData.stampCount % 2 != 0)
    {
        timeDelta += (currentTime->tm_hour * 60);
        timeDelta += currentTime->tm_min;
    }
    
    dayTimeWorked.hours = (char)(timeDelta / 60);
    dayTimeWorked.minutes = (char)(timeDelta % 60);
    
    string_print_daytime(timeWorkedText + sizeof(timeWorkedText) - 6, dayTimeWorked);
    
    text_layer_set_text(textLayerTimeWorked, timeWorkedText);
}