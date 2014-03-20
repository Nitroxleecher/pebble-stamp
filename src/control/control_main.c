#include <pebble.h>
#include "control_main.h"
#include "../model/model.h"
#include "../view/window_main.h"

void control_main_showEditMenu()
{
    showEditMenu();
}

void control_main_showSettingsMenu()
{

}

void control_main_makeStamp()
{
    model_make_stamp();
    window_update_view_main();
}

void control_main_exitApp()
{
    window_stack_pop_all(true);
}