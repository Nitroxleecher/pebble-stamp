#include <pebble.h>
#include <time.h>

#define MAX_TEXT_SIZE 14

#define NUM_TEXT_LAYERS 8

#define MAX_NUM_STAMPS (NUM_TEXT_LAYERS*2)

#define STAMP_EMPTY 0xFF

typedef struct {
    uint8_t hours;
    uint8_t minutes;
} TDayTime;

static Window *mainwindow;

BitmapLayer* backImageLayer;

static TextLayer *text_layer[NUM_TEXT_LAYERS];
static struct tm* currentTime;
static TDayTime stamp[MAX_NUM_STAMPS];

static int nextStampIndex;
static char displayText[NUM_TEXT_LAYERS][MAX_TEXT_SIZE];

const uint32_t KEY_SAVEDATE = 0x01;
const uint32_t KEY_STAMPS = 0x02;
const uint32_t KEY_STAMPINDEX = 0x03;
const uint32_t KEY_TIMETOWORK = 0x04;

static GBitmap* clockIcon;
static GBitmap* settingsIcon;
static GBitmap* editIcon;
static GBitmap* backImage;


ActionBarLayer *action_bar;

static void printTime(char* text, int* index, TDayTime* time)
{
    char highHour = time->hours / 10 + '0';
    char lowHour = time->hours % 10 + '0';
    char highMin = time->minutes / 10 + '0';
    char lowMin = time->minutes % 10 + '0';
 
    text[(*index)] = highHour;
    text[(*index) + 1] = lowHour;
    text[(*index) + 2] = ':';
    text[(*index) + 3] = highMin;
    text[(*index) + 4] = lowMin;
    
    
    (*index) += 5;
}

static void printSpace(char* text, int* index)
{
    text[(*index)++] = ' ';
    text[(*index)++] = '-';
    text[(*index)++] = ' ';
}

static void printTimeTable()
{
    int lineIndex;
    int textIndex = 0;
        
    for (lineIndex = 0; lineIndex < NUM_TEXT_LAYERS; lineIndex++)
    {
        textIndex = 0;
        memset(displayText[lineIndex], 0, sizeof(displayText[lineIndex]));
        
        
        if (stamp[lineIndex*2].hours != STAMP_EMPTY && stamp[lineIndex*2].minutes != STAMP_EMPTY)
        {
            printTime(displayText[lineIndex], &textIndex, &stamp[lineIndex*2]);
        }
        
        if (stamp[lineIndex*2 + 1].hours != STAMP_EMPTY && stamp[lineIndex*2 + 1].minutes != STAMP_EMPTY)
        {
            printSpace(displayText[lineIndex], &textIndex);
            printTime(displayText[lineIndex], &textIndex, &stamp[lineIndex*2 + 1]);
        }

        text_layer_set_text(text_layer[lineIndex], displayText[lineIndex]);
    }
}


static void makeStamp_click_handler(ClickRecognizerRef recognizer, void *context) {
    time_t tmpTimeRead;
    
    if (nextStampIndex >= MAX_NUM_STAMPS) return;
    
    //text_layer_set_text(text_layer, "Up");
    time(&tmpTimeRead);
    currentTime = localtime(&tmpTimeRead);
    
    stamp[nextStampIndex].hours = (uint8_t) currentTime->tm_hour;
    stamp[nextStampIndex].minutes = (uint8_t) currentTime->tm_min;
    
    nextStampIndex++;
    
    printTimeTable();
}


static void settings_click_handler(ClickRecognizerRef recognizer, void *context) {
    //text_layer_set_text(text_layer, "Select");
}

static void edit_click_handler(ClickRecognizerRef recognizer, void *context) {
    nextStampIndex = 0;
    memset(stamp, 0xFF, sizeof(TDayTime) * MAX_NUM_STAMPS);
    
    persist_delete(KEY_SAVEDATE);
    /*
    persist_write_data(KEY_STAMPS, (void*)stamp, sizeof(TDayTime) * MAX_NUM_STAMPS);
    persist_write_data(KEY_STAMPINDEX, (void*)&nextStampIndex, sizeof(nextStampIndex));
    printTimeTable();
    */
}

static void action_bar_click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, settings_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, edit_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, makeStamp_click_handler);
}

static void mainwindow_load(Window *mainwindow) {
    int i;
    
    
    
    Layer *window_layer = window_get_root_layer(mainwindow);
    GRect bounds = layer_get_bounds(window_layer);
    
    layer_add_child(window_layer, (Layer*)backImageLayer);
    
    action_bar = action_bar_layer_create();
    
    action_bar_layer_add_to_window(action_bar, mainwindow);
    
    action_bar_layer_set_icon(action_bar, BUTTON_ID_DOWN, clockIcon);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, settingsIcon);
    action_bar_layer_set_icon(action_bar, BUTTON_ID_UP, editIcon);
    
    action_bar_layer_set_click_config_provider(action_bar, action_bar_click_config_provider);
    
    for (i = 0; i < NUM_TEXT_LAYERS; i++)
    {
        text_layer[i] = text_layer_create((GRect) { .origin = { 30, 0 + i * 14 }, .size = { bounds.size.w- 30 - ACTION_BAR_WIDTH, 14 } });
        text_layer_set_text(text_layer[i], "");
        //text_layer_set_text_alignment(text_layer[i], GTextAlignmentCenter);
        layer_add_child(window_layer, text_layer_get_layer(text_layer[i]));
    }
    printTimeTable();
}

static void mainwindow_unload(Window *mainwindow) {
    int i;
    
    layer_remove_from_parent((Layer*)backImageLayer);
    
    action_bar_layer_remove_from_window(action_bar);
    action_bar_layer_destroy(action_bar);
    
    for (i = 0; i < NUM_TEXT_LAYERS; i++)
    {
        text_layer_destroy(text_layer[i]);
    }
}

static void init(void) {
    
    clockIcon = gbitmap_create_with_resource(RESOURCE_ID_CLOCK_ICON);
    settingsIcon = gbitmap_create_with_resource(RESOURCE_ID_SETTINGS_ICON);
    editIcon = gbitmap_create_with_resource(RESOURCE_ID_EDIT_ICON);
    backImage = gbitmap_create_with_resource(RESOURCE_ID_BACK_IMAGE);
    
    backImageLayer = bitmap_layer_create((GRect) { .origin = { 0, 3 }, .size = { 20, 48 } });
    
    bitmap_layer_set_bitmap(backImageLayer, backImage);
    
    mainwindow = window_create();
    
    //window_set_click_config_provider(window, click_config_provider);
    
    window_set_window_handlers(mainwindow, (WindowHandlers) {
        .load = mainwindow_load,
        .unload = mainwindow_unload,
    });
    const bool animated = true;
    window_stack_push(mainwindow, animated);
}

static void deinit(void) {
    window_destroy(mainwindow);
    
    bitmap_layer_destroy(backImageLayer);
    
    gbitmap_destroy(clockIcon);
    gbitmap_destroy(settingsIcon);
    gbitmap_destroy(editIcon);
    gbitmap_destroy(backImage);
    
}

int main(void) {
    nextStampIndex = 0;
    time_t savedTimestamp;
    struct tm* savedTime;
    time_t tmpTimeRead;
    
    
    time(&tmpTimeRead);
    currentTime = localtime(&tmpTimeRead);
    
    memset(stamp, 0xFF, sizeof(TDayTime) * MAX_NUM_STAMPS);
    
    if (persist_exists(KEY_SAVEDATE) && persist_exists(KEY_STAMPS))
    {
        persist_read_data(KEY_SAVEDATE, (void*)&savedTimestamp, sizeof(savedTimestamp));
        
        savedTime = localtime(&savedTimestamp);
        
        if (savedTime->tm_year == currentTime->tm_year && savedTime->tm_yday == currentTime->tm_yday)
        {
            persist_read_data(KEY_STAMPS, (void*)stamp, sizeof(TDayTime) * MAX_NUM_STAMPS);
            persist_read_data(KEY_STAMPINDEX, (void*)&nextStampIndex, sizeof(nextStampIndex));
        }
        else
        {
            savedTimestamp = tmpTimeRead;
        }
    }
    else
    {
        savedTimestamp = tmpTimeRead;
    }
    
    

    
    
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed mainwindow: %p", mainwindow);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Textsize: %i", MAX_TEXT_SIZE);

    
    
    app_event_loop();
    deinit();
    
    persist_write_data(KEY_SAVEDATE, (void*)&savedTimestamp, sizeof(savedTimestamp));
    persist_write_data(KEY_STAMPS, (void*)stamp, sizeof(TDayTime) * MAX_NUM_STAMPS);
    persist_write_data(KEY_STAMPINDEX, (void*)&nextStampIndex, sizeof(nextStampIndex));
}
