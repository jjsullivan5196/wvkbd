#include "globals.h"
#include "keyboard.h"
#include <stdint.h>

static void
resize(int pixels) {
    uint32_t height = get_current_height();

    if (pixels < 0 && abs(pixels) >= height)
        return;

    set_current_height(height + pixels);
}

void
handle_command(uint32_t code)
{
    switch (code) {
    case IncreaseHeight:
        resize(4); // TODO: This should depend on number of rows
        break;
    case DecrecaseHeight:
        resize(-4);
        break;
    }
}
