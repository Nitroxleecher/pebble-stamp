#include <pebble.h>
#include "model/model.h"
#include "view/window_main.h"

#define log(...) app_log(APP_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)

int main(void) {
    model_init();
    window_init_main();

    app_event_loop();

    window_deinit_main();
    model_deinit();

    log("clean exit");
}
