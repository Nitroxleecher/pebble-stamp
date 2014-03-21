#include "window_edittime.h"
#include "window_main.h"
#include "../model/model.h"

//------------------------------------------------------------------------------
// PUBLIC VARIABLES

Window* window_edittime;

//------------------------------------------------------------------------------
// PRIVATE VARIABLES

static Layer* rootlayer;
static ActionBarLayer* action_bar;

#define NR_TEXT_SIZE 3

static TextLayer* textLayerHours;
static TextLayer* textLayerMinutes;

static char hoursText[NR_TEXT_SIZE];
static char minutesText[NR_TEXT_SIZE];

static InverterLayer* inverterLayerSelected;

static unsigned int editPos;
static TDayTime timeToEdit;

static void (*callbackApply)(TDayTime edited);

static GBitmap* editIcon;
static GBitmap* arrowUpIcon;
static GBitmap* arrowDownIcon;
//------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

static void window_load(Window *wnd);
static void window_unload(Window *wnd);
static void click_config_provider_edittime(void *context);

static void string_print_number(char* text, char nr);
static void window_update_view_edittime();

static void click_handler_up(ClickRecognizerRef recognizer, void *context);
static void click_handler_select(ClickRecognizerRef recognizer, void *context);
static void click_handler_down(ClickRecognizerRef recognizer, void *context);
static void click_handler_back(ClickRecognizerRef recognizer, void *context);
//==============================================================================
// INIT AND CLEANUP FUNCTIONS


//------------------------------------------------------------------------------
// INIT

void window_init_edittime()
{
    window_edittime = window_create();

    window_set_window_handlers(window_edittime, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
}

//------------------------------------------------------------------------------
// DEINIT

void window_deinit_edittime()
{
    window_destroy(window_edittime);
}

//------------------------------------------------------------------------------
// LOAD

#define FONT_SIZE 30

static void window_load(Window *wnd)
{
    GFont textFont;
    GRect bounds;
    
    editPos = 0;
    
    arrowUpIcon = gbitmap_create_with_resource(RESOURCE_ID_ARROW_UP_ICON);
    editIcon = gbitmap_create_with_resource(RESOURCE_ID_EDIT_ICON);
    arrowDownIcon = gbitmap_create_with_resource(RESOURCE_ID_ARROW_DOWN_ICON);
    
    // load system font
    textFont = fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK);
   
    rootlayer = window_get_root_layer(wnd);
    bounds = layer_get_frame(rootlayer);
   
    action_bar = action_bar_layer_create();

    action_bar_layer_add_to_window(action_bar, wnd);

    action_bar_layer_set_click_config_provider(action_bar, click_config_provider_edittime);

    action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, arrowUpIcon);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, editIcon);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, arrowDownIcon);
    
    // LAYERS
    
    inverterLayerSelected = inverter_layer_create(
        (GRect) {.origin = { 0, 0 }, .size = { 100,100} }
    );
    

    textLayerHours = text_layer_create(
        (GRect) {.origin = { 8, (bounds.size.h - (FONT_SIZE)) / 2 }, .size = { 50, FONT_SIZE + 8 } }
    );
    
    textLayerMinutes = text_layer_create(
        (GRect) {.origin = { 64, (bounds.size.h - (FONT_SIZE)) / 2 }, .size = { 50, FONT_SIZE + 8 } }
    );
    

    // set font for all layers
    text_layer_set_font(textLayerHours, textFont);
    text_layer_set_font(textLayerMinutes, textFont);

    // set center alignment for spacer layer
    text_layer_set_text_alignment(textLayerHours, GTextAlignmentCenter);
    text_layer_set_text_alignment(textLayerMinutes, GTextAlignmentCenter);

    // add some random stuff
    
    text_layer_set_text(textLayerHours, "__");
    text_layer_set_text(textLayerMinutes, "__");

    // add all textlayers to root layer
    layer_add_child(rootlayer, text_layer_get_layer(textLayerHours));
    layer_add_child(rootlayer, text_layer_get_layer(textLayerMinutes));
    

    window_update_view_edittime();
}

//------------------------------------------------------------------------------
// UNLOAD

static void window_unload(Window *wnd)
{
    action_bar_layer_destroy(action_bar);
    
    inverter_layer_destroy(inverterLayerSelected);
    
    gbitmap_destroy(arrowUpIcon);
    gbitmap_destroy(editIcon);
    gbitmap_destroy(arrowDownIcon);

    text_layer_destroy(textLayerHours);
    text_layer_destroy(textLayerMinutes);
}

//==============================================================================
// CLICK HANDLERS

static void click_config_provider_edittime(void *context)
{
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 100, click_handler_up);
    window_single_click_subscribe(BUTTON_ID_SELECT, click_handler_select);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 100, click_handler_down);
    window_single_click_subscribe(BUTTON_ID_BACK, click_handler_back);
}

static void click_handler_up(ClickRecognizerRef recognizer, void *context)
{
    if (editPos == 0)
    {
        timeToEdit.hours = (timeToEdit.hours + 1) % 24;
    }
    else
    {
        timeToEdit.minutes = (timeToEdit.minutes + 1) % 24;
    }
    window_update_view_edittime();
}

static void click_handler_select(ClickRecognizerRef recognizer, void *context)
{
    if (editPos == 0)
    {
        editPos = 1;
        window_update_view_edittime();
    }
    else
    {
        callbackApply(timeToEdit);
        window_stack_pop_all(true);
        window_stack_push(window_main, true);
    }
}

static void click_handler_down(ClickRecognizerRef recognizer, void *context)
{
    if (editPos == 0)
    {
        timeToEdit.hours = (timeToEdit.hours - 1) % 24;
    }
    else
    {
        timeToEdit.minutes = (timeToEdit.minutes - 1) % 24;
    }
    window_update_view_edittime();
}

static void click_handler_back(ClickRecognizerRef recognizer, void *context)
{
    if (editPos == 1)
    {
        editPos = 0;
        window_update_view_edittime();
    }
    else
    {
        window_stack_pop(true);
    }
}

//==============================================================================
// APPLICATION FUNCTIONS


static void string_print_number(char* text, char nr)
{
    char highNr = nr / 10 + '0';
    char lowNr = nr % 10 + '0';

    text[0] = highNr;
    text[1] = lowNr;
}

static void window_update_view_edittime()
{
    if (editPos == 0)
    {
        layer_add_child(text_layer_get_layer(textLayerHours), (Layer*)inverterLayerSelected);
    }
    else
    {
        layer_add_child(text_layer_get_layer(textLayerMinutes), (Layer*)inverterLayerSelected);
    }
    
    string_print_number(hoursText, timeToEdit.hours);
    string_print_number(minutesText, timeToEdit.minutes);
    
    text_layer_set_text(textLayerHours, hoursText);
    text_layer_set_text(textLayerMinutes, minutesText);
}

void window_edittime_configure(TDayTime preset,  void (*usecallbackApply)(TDayTime edited))
{
    timeToEdit = preset;
    callbackApply = usecallbackApply;
}