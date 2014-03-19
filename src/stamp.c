#include <pebble.h>
#include <time.h>
#include "ui.h"
#include "application.h"

int main(void) {
    resources_init();
    ui_init();
    application_init();
    
    app_event_loop();
    
    application_deinit();
    
    ui_deinit();
    resources_deinit();
}
