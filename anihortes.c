#include "keyboard.h"
#include "globals.h"
#include <stdint.h>

static bool show_tertiary = false;
static bool show_primary_secondary = true;

static void
resize(int pixels) {
    uint32_t height = get_current_height();

    if (pixels < 0 && abs(pixels) >= height)
        return;

    set_current_height(height + pixels);
}

#define TERTIARY_KEYS_IDX 4

static void
toggle_primary_secondary() {
    show_primary_secondary = !show_primary_secondary;
    if (show_primary_secondary)
    {
        schemes[1].text.bgra[3] = 255;
        schemes[2].text.bgra[3] = 255;
        schemes[3].text.bgra[3] = 255;
    } else {
        schemes[1].text.bgra[3] = 0;
        schemes[2].text.bgra[3] = 0;
        schemes[3].text.bgra[3] = 0;
    }
}

static void
toggle_tertiary() {
    show_tertiary = !show_tertiary;
    if (show_tertiary)
        schemes[TERTIARY_KEYS_IDX].text.bgra[3] = 255;
    else
        schemes[TERTIARY_KEYS_IDX].text.bgra[3] = 0;
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
    case ShowHideTertiaryKeys:
        toggle_tertiary();
        break;
    case ShowHidePrimarySecondaryKeys:
        toggle_primary_secondary();
        break;
    }
}
