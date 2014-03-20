#include <pebble.h>
#include "control_main.h"
#include "../model/model.h"
#include "../view/window_main.h"

static void click_handler_up(ClickRecognizerRef recognizer, void *context);
static void click_handler_select(ClickRecognizerRef recognizer, void *context);
static void click_handler_down(ClickRecognizerRef recognizer, void *context);
static void click_handler_back(ClickRecognizerRef recognizer, void *context);


void click_config_provider_main(void *context)
{
    window_single_click_subscribe(BUTTON_ID_UP, click_handler_up);
    window_single_click_subscribe(BUTTON_ID_SELECT, click_handler_select);
    window_single_click_subscribe(BUTTON_ID_DOWN, click_handler_down);
    window_single_click_subscribe(BUTTON_ID_BACK, click_handler_back);
}

static void click_handler_up(ClickRecognizerRef recognizer, void *context)
{

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