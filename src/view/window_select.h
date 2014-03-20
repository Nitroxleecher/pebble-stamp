#include <pebble.h>

typedef enum {
    SELECTMODE_ADD,
    SELECTMODE_REMOVE
} TSelectMode;

extern Window* window_select;

extern void window_select_set_mode(TSelectMode mode);

void window_init_select();
void window_deinit_select();

void window_update_view_select();
