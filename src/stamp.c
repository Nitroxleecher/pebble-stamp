#include <pebble.h>
#include "model/model.h"
#include "view/window_main.h"
#include "view/window_editmenu.h"
#include "view/window_select.h"

int main(void) {
    model_init();
    window_init_main();
    window_init_editmenu();
    window_init_select();
    
    app_event_loop();

    model_deinit();
    window_deinit_main();
    window_deinit_editmenu();
    window_deinit_select();
    
    log("clean exit");
}
