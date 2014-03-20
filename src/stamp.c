#include <pebble.h>
#include "model/model.h"
#include "view/window_main.h"
#include "view/window_editmenu.h"

#define log(...) app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

int main(void) {
    model_init();
    window_init_main();
    window_init_editmenu();
    app_event_loop();

    window_deinit_main();
    window_deinit_editmenu();
    model_deinit();

    log("clean exit");
}
