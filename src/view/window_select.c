#include "window_select.h"
#include "window_main.h"
#include "window_edittime.h"

#include "model/model.h"
#include <time.h>


//------------------------------------------------------------------------------
// PUBLIC VARIABLES

Window* window_select;

//------------------------------------------------------------------------------
// PRIVATE VARIABLES

static Layer* rootlayer;
static ActionBarLayer* action_bar;

static TextLayer *textLayerTime[MAX_NUM_STAMPS];
static TextLayer *textLayerSpacer[MAX_NUM_TIME_PERIODS];

static InverterLayer* inverterLayer;

static TSelectMode selectMode;
static uint editPos;

#define STAMP_TEXT_SIZE 6
static char stampText[MAX_NUM_STAMPS][STAMP_TEXT_SIZE];

static GBitmap* editIcon;
static GBitmap* arrowUpIcon;
static GBitmap* arrowDownIcon;
//------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

static void window_load(Window *wnd);
static void window_unload(Window *wnd);
static void string_print_daytime(char* text, TDayTime time);
static void click_config_provider_select(void *context);

static void editTimeCallback(TDayTime timeToInsert);

static void click_handler_up(ClickRecognizerRef recognizer, void *context);
static void click_handler_select(ClickRecognizerRef recognizer, void *context);
static void click_handler_down(ClickRecognizerRef recognizer, void *context);
static void click_handler_back(ClickRecognizerRef recognizer, void *context);
//==============================================================================
// INIT AND CLEANUP FUNCTIONS


//------------------------------------------------------------------------------
// INIT

void window_init_select()
{
    window_select = window_create();

    window_set_window_handlers(window_select, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
}

//------------------------------------------------------------------------------
// DEINIT

void window_deinit_select()
{
    window_destroy(window_select);
}

//------------------------------------------------------------------------------
// LOAD

#define FONT_SIZE 18

static void window_load(Window *wnd)
{
    GFont gothic18Font;
    int i;
    //GRect bounds;
    
    editPos = 0;
    
    arrowUpIcon = gbitmap_create_with_resource(RESOURCE_ID_ARROW_UP_ICON);
    editIcon = gbitmap_create_with_resource(RESOURCE_ID_EDIT_ICON);
    arrowDownIcon = gbitmap_create_with_resource(RESOURCE_ID_ARROW_DOWN_ICON);
    
    // load system font
    gothic18Font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

    inverterLayer = inverter_layer_create(
        (GRect) {.origin = { 0, 6 }, .size = { 100,100} }
    );

    rootlayer = window_get_root_layer(wnd);
    //bounds = layer_get_frame(rootlayer);
    
    action_bar = action_bar_layer_create();

    action_bar_layer_add_to_window(action_bar, wnd);

    action_bar_layer_set_click_config_provider(action_bar, click_config_provider_select);

    action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, arrowUpIcon);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, editIcon);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, arrowDownIcon);
    
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

    window_update_view_select();
}

//------------------------------------------------------------------------------
// UNLOAD

static void window_unload(Window *wnd)
{
    int i;

    action_bar_layer_destroy(action_bar);
    inverter_layer_destroy(inverterLayer);
    
    gbitmap_destroy(arrowUpIcon);
    gbitmap_destroy(editIcon);
    gbitmap_destroy(arrowDownIcon);

    for (i = 0; i < MAX_NUM_TIME_PERIODS; i++)
    {
        text_layer_destroy(textLayerTime[i*2]);
        text_layer_destroy(textLayerSpacer[i]);
        text_layer_destroy(textLayerTime[i * 2 + 1]);

    }
}

void window_select_set_mode(TSelectMode mode)
{
    selectMode = mode;
}

//==============================================================================
// CLICK HANDLERS

static void click_config_provider_select(void *context)
{
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, click_handler_up);
    window_single_click_subscribe(BUTTON_ID_SELECT, click_handler_select);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, click_handler_down);
    window_single_click_subscribe(BUTTON_ID_BACK, click_handler_back);
}

static void click_handler_up(ClickRecognizerRef recognizer, void *context)
{
    if (editPos > 0) editPos--;
    window_update_view_select();
}

static void click_handler_select(ClickRecognizerRef recognizer, void *context)
{
    TDayTime timeToEdit;
    time_t currentTimestamp;
    struct tm* currentTime;

    if (selectMode == SELECTMODE_ADD)
    {
    
        time(&currentTimestamp);
        currentTime = localtime(&currentTimestamp);
        
        timeToEdit.hours = (uint8_t) currentTime->tm_hour;
        timeToEdit.minutes = (uint8_t) currentTime->tm_min;
        
        window_edittime_configure(timeToEdit, &editTimeCallback);
        window_stack_push(window_edittime, true);
    }
    else if (selectMode == SELECTMODE_REMOVE)
    {
        model_remove_time(editPos);
        window_stack_pop_all(true);
        window_stack_push(window_main, true);
    }
}

static void click_handler_down(ClickRecognizerRef recognizer, void *context)
{
    if (selectMode == SELECTMODE_ADD)
    {
        if (editPos < appData.stampCount) editPos++;
    }
    else if (selectMode == SELECTMODE_REMOVE)
    {
        if (editPos < appData.stampCount - 1) editPos++;
    }
    window_update_view_select();
}

static void click_handler_back(ClickRecognizerRef recognizer, void *context)
{
    window_stack_pop(true);
}

//==============================================================================
// APPLICATION FUNCTIONS

static void editTimeCallback(TDayTime timeToInsert)
{
    model_insert_time(editPos, timeToInsert);
}

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

void window_update_view_select()
{
    uint line;
    uint stampIndex;
    uint textIndex;
    
    for (stampIndex = 0, textIndex = 0; stampIndex < MAX_NUM_STAMPS; stampIndex++, textIndex++)
    {
        if (selectMode == SELECTMODE_ADD && stampIndex == editPos)
        {
            memset(stampText[textIndex], 0, STAMP_TEXT_SIZE);
            textIndex++;
        }
        if (stampIndex < appData.stampCount)
        {
            string_print_daytime(stampText[textIndex], appData.stamp[stampIndex]);
        }
        else if (textIndex < MAX_NUM_STAMPS)
        {
            memset(stampText[textIndex], 0, STAMP_TEXT_SIZE);
        }
    }

    for (line = 0; line < MAX_NUM_TIME_PERIODS; line++)
    {
        text_layer_set_text(textLayerTime[line * 2], stampText[line * 2]);

        if (line * 2 + 1 <  appData.stampCount + (selectMode == SELECTMODE_ADD ? 1 : 0))
        {
            text_layer_set_text(textLayerSpacer[line], "-");
        }
        else
        {
            text_layer_set_text(textLayerSpacer[line], "");
        }

         text_layer_set_text(textLayerTime[line * 2 + 1], stampText[line * 2 + 1]);
    }
    
    layer_add_child(text_layer_get_layer(textLayerTime[editPos]), (Layer*)inverterLayer);
}