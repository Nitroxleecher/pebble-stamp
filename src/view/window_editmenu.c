#include "window_main.h"
#include "model/model.h"
#include "window_select.h"

#define EDIT_MENU_NUM_SECTIONS 1
#define EDIT_MENU_MAX_NUM_ITEMS 3

//------------------------------------------------------------------------------
// PUBLIC VARIABLES

Window* window_editmenu;

//------------------------------------------------------------------------------
// PRIVATE VARIABLES

static Layer* rootlayer;
static MenuLayer* edit_menu_layer;

//------------------------------------------------------------------------------
// PRIVATE FUNCTIONS

static void window_load(Window *wnd);
static void window_unload(Window *wnd);

static uint16_t edit_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data);
static uint16_t edit_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
static int16_t edit_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data);
static void edit_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data);
static void edit_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data);
void edit_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data);

typedef struct {
    int menuitemCount;
    void (*menuitem_draw[EDIT_MENU_MAX_NUM_ITEMS])(GContext* ctx, const Layer *cell_layer);
    void (*menuitem_action[EDIT_MENU_MAX_NUM_ITEMS])();
} TMenuItems;

static TMenuItems menuItems;

static void menuitem_draw_add();
static void menuitem_draw_remove();
static void menuitem_draw_remove_all();
static void menuitem_action_add();
static void menuitem_action_remove();
static void menuitem_action_remove_all();

//==============================================================================
// INIT AND CLEANUP FUNCTIONS


//------------------------------------------------------------------------------
// INIT

void window_init_editmenu()
{
    window_editmenu = window_create();

    window_set_window_handlers(window_editmenu, (WindowHandlers) {
        .load = window_load,
        .unload = window_unload,
    });
}

//------------------------------------------------------------------------------
// DEINIT

void window_deinit_editmenu()
{
    window_destroy(window_editmenu);
}

//------------------------------------------------------------------------------
// LOAD

#define FONT_SIZE 18

static void window_load(Window *wnd)
{
    GRect bounds;
    
    rootlayer = window_get_root_layer(wnd);
    bounds = layer_get_frame(rootlayer);

    edit_menu_layer = menu_layer_create(bounds);
    
    if(appData.stampCount == 0)
    {
        menuItems.menuitemCount = 1;
        menuItems.menuitem_draw[0] = &menuitem_draw_add;
        menuItems.menuitem_action[0] = &menuitem_action_add;
    }
    else if (appData.stampCount == MAX_NUM_STAMPS)
    {
        menuItems.menuitemCount = 2;
        menuItems.menuitem_draw[0] = &menuitem_draw_remove;
        menuItems.menuitem_action[0] = &menuitem_action_remove;
        menuItems.menuitem_draw[1] = &menuitem_draw_remove_all;
        menuItems.menuitem_action[1] = &menuitem_action_remove_all;
    }
    else
    {
        menuItems.menuitemCount = 3;
        menuItems.menuitem_draw[0] = &menuitem_draw_add;
        menuItems.menuitem_action[0] = &menuitem_action_add;
        menuItems.menuitem_draw[1] = &menuitem_draw_remove;
        menuItems.menuitem_action[1] = &menuitem_action_remove;
        menuItems.menuitem_draw[2] = &menuitem_draw_remove_all;
        menuItems.menuitem_action[2] = &menuitem_action_remove_all;
    }
    
    
    // Set all the callbacks for the menu layer
    menu_layer_set_callbacks(edit_menu_layer, NULL, (MenuLayerCallbacks){
        .get_num_sections = edit_menu_get_num_sections_callback,
        .get_num_rows = edit_menu_get_num_rows_callback,
        .get_header_height = edit_menu_get_header_height_callback,
        .draw_header = edit_menu_draw_header_callback,
        .draw_row = edit_menu_draw_row_callback,
        .select_click = edit_menu_select_callback,
    });
    
    // Bind the menu layer's click config provider to the window for interactivity
    menu_layer_set_click_config_onto_window(edit_menu_layer, wnd);
    
    // Add it to the window for display
    layer_add_child(rootlayer, menu_layer_get_layer(edit_menu_layer));
    
}

//------------------------------------------------------------------------------
// UNLOAD

static void window_unload(Window *wnd)
{
    menu_layer_destroy(edit_menu_layer);
}


//==============================================================================
// MENU CALLBACKS

static uint16_t edit_menu_get_num_sections_callback(MenuLayer *menu_layer, void *data)
{
    return EDIT_MENU_NUM_SECTIONS;
}

static uint16_t edit_menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
    return menuItems.menuitemCount;
}

static int16_t edit_menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data)
{
    // This is a define provided in pebble.h that you may use for the default height
    return MENU_CELL_BASIC_HEADER_HEIGHT;
}

// Here we draw what each header is
static void edit_menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data)
{
    menu_cell_basic_header_draw(ctx, cell_layer, "Edit stamps");
}

// This is the menu item draw callback where you specify what each item should look like
static void edit_menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data)
{
    (*(menuItems.menuitem_draw[cell_index->row]))(ctx, cell_layer);
}

// Here we capture when a user selects a menu item
void edit_menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data)
{
    (*(menuItems.menuitem_action[cell_index->row]))();
}

//==============================================================================
// APPLICATION FUNCTIONS


static void menuitem_draw_add(GContext* ctx, const Layer *cell_layer)
{
    menu_cell_basic_draw(ctx, cell_layer, "Add", "Add a stamp", NULL);
}

static void menuitem_draw_remove(GContext* ctx, const Layer *cell_layer)
{
    menu_cell_basic_draw(ctx, cell_layer, "Remove", "Delete a stamp", NULL);
}

static void menuitem_draw_remove_all(GContext* ctx, const Layer *cell_layer)
{
    menu_cell_basic_draw(ctx, cell_layer, "Remove all", "Delete all stamps", NULL);
}

static void menuitem_action_add()
{
    window_select_set_mode(SELECTMODE_ADD);
    window_stack_push(window_select, true);
}

static void menuitem_action_remove()
{
    window_select_set_mode(SELECTMODE_REMOVE);
    window_stack_push(window_select, true);
}

static void menuitem_action_remove_all()
{
    model_remove_all();
    window_stack_pop_all(true);
    window_stack_push(window_main, true);
}