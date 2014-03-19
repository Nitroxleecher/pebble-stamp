#include "ui.h"
#include "configuration.h"
#include "application.h"

Window *mainwindow;

TextLayer *tableLayersTime[MAXNUM_TIMESPANS * 2];
TextLayer *tableLayersSpacer[MAXNUM_TIMESPANS];

GBitmap* clockIcon;
GBitmap* settingsIcon;
GBitmap* editIcon;
GBitmap* backImage;

InverterLayer* invLayer;

void resources_init()
{
    clockIcon = gbitmap_create_with_resource(RESOURCE_ID_CLOCK_ICON);
    settingsIcon = gbitmap_create_with_resource(RESOURCE_ID_SETTINGS_ICON);
    editIcon = gbitmap_create_with_resource(RESOURCE_ID_EDIT_ICON);
    //backImage = gbitmap_create_with_resource(RESOURCE_ID_BACK_IMAGE);
}

void resources_deinit()
{
    gbitmap_destroy(clockIcon);
    gbitmap_destroy(settingsIcon);
    gbitmap_destroy(editIcon);
    //gbitmap_destroy(backImage);
}


// MAIN WINDOW -----------------------------------------------------------------

void mainwindow_init()
{
    mainwindow = window_create();
    
    //window_set_click_config_provider(window, click_config_provider);
    
    window_set_window_handlers(mainwindow, (WindowHandlers) {
        .load = mainwindow_load,
        .unload = mainwindow_unload,
    });
    
    const bool animated = true;
    window_stack_push(mainwindow, animated);
}

void mainwindow_deinit(void)
{
    window_destroy(mainwindow);
}

#define FONT_SIZE 19

void mainwindow_load(Window *mainwindow)
{
    GFont gothic18Font;
    int i;
    Layer *window_layer = window_get_root_layer(mainwindow);
    //GRect bounds = layer_get_bounds(window_layer);
    
    invLayer = inverter_layer_create(
        (GRect) {.origin = { 0, 6 }, .size = { 100,100} }
    );
    
    gothic18Font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
    
    for (i = 0; i < MAXNUM_TIMESPANS; i++)
    {
        tableLayersTime[i*2] = text_layer_create(
            (GRect) {.origin = { 30, 0 + i * (FONT_SIZE) }, .size = { 37, FONT_SIZE } }
        );
        
        tableLayersSpacer[i] = text_layer_create(
            (GRect) {.origin = { 67, 0 + i * (FONT_SIZE) }, .size = { 10, FONT_SIZE } }
        );
        
        tableLayersTime[i * 2 + 1] = text_layer_create(
            (GRect) {.origin = { 77, 0 + i * (FONT_SIZE) }, .size = { 37, FONT_SIZE } }
        );
        
        text_layer_set_font(tableLayersTime[i*2], gothic18Font);
        text_layer_set_font(tableLayersTime[i*2 + 1], gothic18Font);
        text_layer_set_font(tableLayersSpacer[i], gothic18Font);
        
        text_layer_set_text_alignment(tableLayersSpacer[i], GTextAlignmentCenter);
        
        text_layer_set_text(tableLayersTime[i*2], "12:34");
        text_layer_set_text(tableLayersTime[i*2 + 1], "56:78");
        text_layer_set_text(tableLayersSpacer[i], "-");
        
        layer_add_child(window_layer, text_layer_get_layer(tableLayersTime[i*2]));
        layer_add_child(window_layer, text_layer_get_layer(tableLayersTime[i*2 + 1]));
        layer_add_child(window_layer, text_layer_get_layer(tableLayersSpacer[i]));
    }
}

void mainwindow_unload(Window *mainwindow)
{
    int i;
    
    inverter_layer_destroy(invLayer);
    
    for (i = 0; i < MAXNUM_TIMESPANS; i++)
    {
        text_layer_destroy(tableLayersTime[i*2]);
        text_layer_destroy(tableLayersSpacer[i]);
        text_layer_destroy(tableLayersTime[i * 2 + 1]);
        
    }
}

// TEXT

void setTableTimeText(int index, const char* text)
{
    text_layer_set_text(tableLayersTime[index], text);
}

void setTableSpacerText(int index, const char* text)
{
    text_layer_set_text(tableLayersSpacer[index], text);
}

void higlightTableTimeLayer(int index)
{
    layer_remove_from_parent((Layer*) invLayer);
    layer_add_child(text_layer_get_layer(tableLayersTime[index]), (Layer*)invLayer);
}


void ui_init()
{
    resources_init();
    mainwindow_init();
}

void ui_deinit()
{
    mainwindow_deinit();
    resources_deinit();
}