#include <linux/input-event-codes.h>

#include "keyboard.h"
/* constants */
/* how tall the keyboard should be by default (can be overriden) */
#define KBD_PIXEL_HEIGHT 280

/* how tall the keyboard should be by default (can be overriden) */
#define KBD_PIXEL_LANDSCAPE_HEIGHT 240

/* spacing around each key */
#define KBD_KEY_BORDER 2

/* layout declarations */
enum layout_id {
    Anihortes = 0,
    NumPad,
    Navigation,
    Index,
    NumLayouts,
};


enum keyboard_command {
    IncreaseHeight,
    DecrecaseHeight,
    ShowHideTertiaryKeys,
    ShowHidePrimarySecondaryKeys,
};

static struct key keys_anihortes[], keys_index[], keys_numpad[], keys_navigation[];

static struct layout layouts[NumLayouts] = {
    [Anihortes] = {keys_anihortes, "latin", "anihortes", true}, // second parameter is the keymap name
                                                                // third parameter is the layout name
                                                                // last parameter indicates if it's an alphabetical/primary layout
    [NumPad] = {keys_numpad, "latin", "numpad", true},
    [Navigation] = {keys_navigation, "latin", "navigation", false},

    [Index] = {keys_index,"latin","index", false},
};

#define UNUSED_KEY {"", "", 0.0, Pad}

// How to do dead keys?
// This keyboard uses a keymap that maps <I147>, <I148>, … to self defined keys.

// corners of key A (top left)
static struct key key_anihortes_a_north      = {"Esc", "Esc", 1.0, Code, KEY_ESC, .scheme = 4};
static struct key key_anihortes_a_north_east = {"ı", "İ", 1.0, Copy, 0x0131, 0, 0x0130, .scheme = 2};
static struct key key_anihortes_a_east       = {"-", "÷", 1.0, Copy, 0x002D, 0, 0x00F7, .scheme = 4};
static struct key key_anihortes_a_south_east = {"v", "V", 1.0, Code, KEY_V, .scheme = 2};
static struct key key_anihortes_a_south      = {"ä", "Ä", 1.0, Copy, 0x00E4, 0, 0x00C4, .scheme = 2};
static struct key key_anihortes_a_south_west = {"$", "¥", 1.0, Copy, 0x0024, 0, 0x00A5, .scheme = 4};
static struct key key_anihortes_a_west       = {"ß", "ẞ", 1.0, Copy, 0x00DF, 0, 0x1E9E, .scheme = 2};
static struct key key_anihortes_a_north_west = {"🅲", "🅲", 1.0, Compose, .scheme = 4};
static struct key key_anihortes_a_long_tap   = {"1", "1", 1.0, Code, KEY_1};

// corners of key N (top center)
static struct key key_anihortes_n_north      = {"^", "ˇ", 1.0, Code, 147-8, .scheme = 4};
static struct key key_anihortes_n_north_east = {"´", "’", 1.0, Code, 148-8, .scheme = 4};
static struct key key_anihortes_n_east       = {"!", "¡", 1.0, Copy, 0x0021, 0, 0x00A1, .scheme = 4};
static struct key key_anihortes_n_south_east = {"\\", "—", 1.0, Copy, 0x005C, 0, 0x2014, .scheme = 4};
static struct key key_anihortes_n_south      = {"l", "L", 1.0, Code, KEY_L, .scheme = 2};
static struct key key_anihortes_n_south_west = {"/", "–", 1.0, Copy, 0x002F, 0, 0x2013, .scheme = 4};
static struct key key_anihortes_n_west       = {"+", "×", 1.0, Copy, 0x002B, 0, 0x00D7, .scheme = 4};
static struct key key_anihortes_n_north_west = {"`", "‘", 1.0, Code, 149-8, .scheme = 4};
static struct key key_anihortes_n_long_tap   = {"2", "2", 1.0, Code, KEY_2};

// corners of key I (top right)
static struct key key_anihortes_i_north      = {"↑", "↑", 1.0, Code, KEY_UP, .scheme = 4};
static struct key key_anihortes_i_north_east = {"⇈", "⇈", 1.0, Code, KEY_PAGEUP, .scheme = 4};
static struct key key_anihortes_i_east       = {"ü", "Ü", 1.0, Copy, 0x00FC, 0, 0x00DC, .scheme = 2};
static struct key key_anihortes_i_south_east = {"€", "£", 1.0, Copy, 0x20AC, 0, 0x00A3, .scheme = 4};
static struct key key_anihortes_i_south      = {"=", "±", 1.0, Copy, 0x003D, 0, 0x00B1, .scheme = 4};
static struct key key_anihortes_i_south_west = {"x", "X", 1.0, Code, KEY_X, .scheme = 2};
static struct key key_anihortes_i_west       = {"?", "¿", 1.0, Copy, 0x003F, 0, 0x00BF, .scheme = 4};
static struct key key_anihortes_i_north_west = {"ö", "Ö", 1.0, Copy, 0x00F6, 0, 0x00D6, .scheme = 2};
static struct key key_anihortes_i_long_tap   = {"3", "3", 1.0, Code, KEY_3};

// corners of key H (middle left)
static struct key key_anihortes_h_north      = {"ğ", "Ğ", 1.0, Copy, 0x011F, 0, 0x011E, .scheme = 2};
static struct key key_anihortes_h_north_east = {"%", "‰", 1.0, Copy, 0x0025, 0, 0x2030, .scheme = 4};
static struct key key_anihortes_h_east       = {"k", "K", 1.0, Code, KEY_K, .scheme = 2};
static struct key key_anihortes_h_south_east = {"_", "¬", 1.0, Copy, 0x005F, 0, 0x00AC, .scheme = 4};
static struct key key_anihortes_h_south      = {"ç", "Ç", 1.0, Copy, 0x00E7, 0, 0x00C7, .scheme = 2};
static struct key key_anihortes_h_south_west = {"[", "{", 1.0, Code, KEY_LEFTBRACE, .scheme = 4};
static struct key key_anihortes_h_west       = {"⇤", "⇤", 1.0, Code, KEY_HOME, .scheme = 4};
static struct key key_anihortes_h_north_west = {"(", "9", 1.0, Code, KEY_9, 0, Shift, .scheme = 4};
static struct key key_anihortes_h_long_tap   = {"4", "4", 1.0, Code, KEY_4};

// corners of key O (middle center)
static struct key key_anihortes_o_north      = {"u", "U", 1.0, Code, KEY_U, .scheme = 2};
static struct key key_anihortes_o_north_east = {"p", "P", 1.0, Code, KEY_P, .scheme = 2};
static struct key key_anihortes_o_east       = {"b", "B", 1.0, Code, KEY_B, .scheme = 2};
static struct key key_anihortes_o_south_east = {"j", "J", 1.0, Code, KEY_J, .scheme = 2};
static struct key key_anihortes_o_south      = {"d", "D", 1.0, Code, KEY_D, .scheme = 2};
static struct key key_anihortes_o_south_west = {"g", "G", 1.0, Code, KEY_G, .scheme = 2};
static struct key key_anihortes_o_west       = {"c", "C", 1.0, Code, KEY_C, .scheme = 2};
static struct key key_anihortes_o_north_west = {"q", "Q", 1.0, Code, KEY_Q, .scheme = 2};
static struct key key_anihortes_o_long_tap   = {"5", "5", 1.0, Code, KEY_5};

// corners of key R (middle right)
static struct key key_anihortes_r_north      = {"⇧", "⇧", 1.0, Mod, Shift, .scheme = 2}; // TODO: Check if drag-return can be interpreted as CapsLock
static struct key key_anihortes_r_north_east = {")", "0", 1.0, Code, KEY_0, 0, Shift, .scheme = 4};
static struct key key_anihortes_r_east       = {"⇥", "⇥", 1.0, Code, KEY_END, .scheme = 4};
static struct key key_anihortes_r_south_east = {"]", "}", 1.0, Code, KEY_RIGHTBRACE, .scheme = 4};
static struct key key_anihortes_r_south      = {"CpL", "CpL", 1.0, Mod, CapsLock, .scheme = 2};
static struct key key_anihortes_r_south_west = {"@", "ᵃ", 1.0, Copy, 0x0040, 0 ,0x1D43, .scheme = 4};
static struct key key_anihortes_r_west       = {"m", "M", 1.0, Code, KEY_M, .scheme = 2};
static struct key key_anihortes_r_north_west = {"|", "¶", 1.0, Copy, 0x07C, 0, 0x00B6, .scheme = 4};
static struct key key_anihortes_r_long_tap   = {"6", "6", 1.0, Code, KEY_6};

// corners of key T (bottom left)
static struct key key_anihortes_t_north      = {"̈ ", "˝", 1.0, Code, 151-8, .scheme = 4};
static struct key key_anihortes_t_north_east = {"y", "Y", 1.0, Code, KEY_Y, .scheme = 2};
static struct key key_anihortes_t_east       = {"*", "†", 1.0, Copy, 0x002A, 0, 0x2020, .scheme = 4};
static struct key key_anihortes_t_south_east = {"↹", "↹", 1.0, Code, KEY_TAB, .scheme = 4};
static struct key key_anihortes_t_south      = UNUSED_KEY;
static struct key key_anihortes_t_south_west = {"⌫", "⌫", 1.0, Code, KEY_BACKSPACE, .scheme = 4};
static struct key key_anihortes_t_west       = {"<", "‹", 1.0, Copy, 0x003C, 0, 0x2039, .scheme = 4};
static struct key key_anihortes_t_north_west = {"~", "̃ ", 1.0, Code, 152-8, .scheme = 4};
static struct key key_anihortes_t_long_tap   = {"7", "7", 1.0, Code, KEY_7};

// corners of key E (bottom center)
static struct key key_anihortes_e_north      = {"w", "W", 1.0, Code, KEY_W, .scheme = 2};
static struct key key_anihortes_e_north_east = {"'", "”", 1.0, Copy, 0x0027, 0, 0x201D, .scheme = 3};
static struct key key_anihortes_e_east       = {"z", "Z", 1.0, Code, KEY_Z, .scheme = 2};
static struct key key_anihortes_e_south_east = {":", "„", 1.0, Copy, 0x003A, 0, 0x201E, .scheme = 3};
static struct key key_anihortes_e_south      = {".", "…", 1.0, Copy, 0x002E, 0, 0x2026, .scheme = 3};
static struct key key_anihortes_e_south_west = {",", "‚", 1.0, Copy, 0x002C, 0, 0x201A, .scheme = 3};
static struct key key_anihortes_e_west       = {"ş", "Ş", 1.0, Copy, 0x015F, 0, 0x015E, .scheme = 2};
static struct key key_anihortes_e_north_west = {"\"", "“", 1.0, Copy, 0x0022, 0, 0x201C, .scheme = 4};
static struct key key_anihortes_e_long_tap   = {"8", "8", 1.0, Code, KEY_8};

// corners of key S (bottom right)
static struct key key_anihortes_s_north      = {"&", "§", 1.0, Copy, 0x0026, 0, 0x00A7, .scheme = 4};
static struct key key_anihortes_s_north_east = {"°", "°", 1.0, Code, 153-8, .scheme = 4};
static struct key key_anihortes_s_east       = {">", "›", 1.0, Copy, 0x003E, 0, 0x203A, .scheme = 4};
static struct key key_anihortes_s_south_east = {"⇊", "⇊", 1.0, Code, KEY_PAGEDOWN, .scheme = 4};
static struct key key_anihortes_s_south      = {"↓", "↓", 1.0, Code, KEY_DOWN, .scheme = 4};
static struct key key_anihortes_s_south_west = {";", ":", 1.0, Code, KEY_SEMICOLON, .scheme = 4};
static struct key key_anihortes_s_west       = {"#", "¥", 1.0, Copy, 0x0023, 0, 0x00A5, .scheme = 4};
static struct key key_anihortes_s_north_west = {"f", "F", 1.0, Code, KEY_F, .scheme = 2};
static struct key key_anihortes_s_long_tap   = {"9", "9", 1.0, Code, KEY_9};

// corners of key settings key (special key on the right with hand symbol)
static struct key key_anihortes_settings_north      = {"", "", 1.0, Command, IncreaseHeight, 0, DecrecaseHeight, .scheme = 4};
static struct key key_anihortes_settings_north_east = UNUSED_KEY;
static struct key key_anihortes_settings_east       = UNUSED_KEY;
static struct key key_anihortes_settings_south_east = UNUSED_KEY;
static struct key key_anihortes_settings_south      = {"", "", 1.0, Command, DecrecaseHeight, 0, IncreaseHeight, .scheme = 4};
static struct key key_anihortes_settings_south_west = UNUSED_KEY;
static struct key key_anihortes_settings_west       = UNUSED_KEY;
static struct key key_anihortes_settings_north_west = UNUSED_KEY;
static struct key key_anihortes_settings_long_tap   = UNUSED_KEY;

// corners of layout key (special key on the right in the second row)
static struct key key_anihortes_navigation_north      = {"N", "N", 1.0, Layout, 0, &layouts[Navigation], .scheme = 4};
static struct key key_anihortes_navigation_north_east = UNUSED_KEY;
static struct key key_anihortes_navigation_east       = UNUSED_KEY;
static struct key key_anihortes_navigation_south_east = UNUSED_KEY;
static struct key key_anihortes_navigation_south      = UNUSED_KEY;
static struct key key_anihortes_navigation_south_west = UNUSED_KEY;
static struct key key_anihortes_navigation_west       = {"№", "№", 1.0, Layout, 0, &layouts[NumPad], .scheme = 4};
static struct key key_anihortes_navigation_north_west = UNUSED_KEY;
static struct key key_anihortes_navigation_long_tap   = {"abc", "abc", 1.0, Layout, 0, &layouts[Index], .scheme = 4};

static struct key key_anihortes_space_north      = {"", "", 1.0, Command, ShowHideTertiaryKeys, 0, ShowHidePrimarySecondaryKeys, .scheme = 5};
static struct key key_anihortes_space_north_east = {"Ctr", "Ctr", 1.0, Mod, Ctrl, .scheme = 1};
static struct key key_anihortes_space_east       = {"→", "→", 1.0, Code, KEY_RIGHT, .scheme = 4};
static struct key key_anihortes_space_south_east = {"Sup", "Sup", 1.0, Mod, Super, .scheme = 1};
static struct key key_anihortes_space_south      = UNUSED_KEY;
static struct key key_anihortes_space_south_west = UNUSED_KEY;
static struct key key_anihortes_space_west       = {"←", "←", 1.0, Code, KEY_LEFT, .scheme = 4};
static struct key key_anihortes_space_north_west = {"Alt", "Alt", 1.0, Mod, Alt, .scheme = 1};
static struct key key_anihortes_space_long_tap   = {"0", "0", 1.0, Code, KEY_0};

static struct key key_anihortes_backspace_north      = {"⌦", "⌦", 1.0, Code, KEY_DELETE, .scheme = 4};
static struct key key_anihortes_backspace_north_east = UNUSED_KEY;
static struct key key_anihortes_backspace_east       = UNUSED_KEY;
static struct key key_anihortes_backspace_south_east = UNUSED_KEY;
static struct key key_anihortes_backspace_south      = UNUSED_KEY;
static struct key key_anihortes_backspace_south_west = UNUSED_KEY;
static struct key key_anihortes_backspace_west       = {"⌫", "⌫", 1.0, Code, KEY_BACKSPACE, 0, Ctrl, .scheme = 4};
static struct key key_anihortes_backspace_north_west = UNUSED_KEY;
static struct key key_anihortes_backspace_long_tap   = UNUSED_KEY;

/* key layouts
 *
 * define keys like:
 *
 *  `{
 *     "label",
 *     "SHIFT_LABEL",
 *     1,
 *     [Code, Mod, Layout, EndRow, Last],
 *     [KEY_CODE, Modifier],
 *     [&layout]
 *  },`
 *
 * - label: normal label for key
 *
 * - shift_label: label for key in shifted (uppercase) layout
 *
 * - width: column width of key
 *
 * - type: what kind of action this key peforms (emit keycode, toggle modifier,
 *   switch layout, or end the layout)
 *
 * - code: key scancode or modifier name (see
 *   `/usr/include/linux/input-event-codes.h` for scancode names, and
 *   `keyboard.h` for modifiers)
 *
 * - layout: layout to switch to when key is pressed
 */
static struct key keys_anihortes[] = {
    {"", "", 70.0, Pad},
    {"a", "A", -2.0, Code, KEY_A, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_a_north,
        .north_east = &key_anihortes_a_north_east,
        .east       = &key_anihortes_a_east,
        .south_east = &key_anihortes_a_south_east,
        .south      = &key_anihortes_a_south,
        .south_west = &key_anihortes_a_south_west,
        .west       = &key_anihortes_a_west,
        .north_west = &key_anihortes_a_north_west,
        .long_tap   = &key_anihortes_a_long_tap,
    },
    {"n", "N", -2.0, Code, KEY_N, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_n_north,
        .north_east = &key_anihortes_n_north_east,
        .east       = &key_anihortes_n_east,
        .south_east = &key_anihortes_n_south_east,
        .south      = &key_anihortes_n_south,
        .south_west = &key_anihortes_n_south_west,
        .west       = &key_anihortes_n_west,
        .north_west = &key_anihortes_n_north_west,
        .long_tap   = &key_anihortes_n_long_tap,
    },
    {"i", "I", -2.0, Code, KEY_I, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_i_north,
        .north_east = &key_anihortes_i_north_east,
        .east       = &key_anihortes_i_east,
        .south_east = &key_anihortes_i_south_east,
        .south      = &key_anihortes_i_south,
        .south_west = &key_anihortes_i_south_west,
        .west       = &key_anihortes_i_west,
        .north_west = &key_anihortes_i_north_west,
        .long_tap   = &key_anihortes_i_long_tap,
    },
    {"☝", "☝", -2.0, Code, KEY_X, .scheme = 0, .shape = OneSquare, // TODO: Hide Keyboard?
        .north      = &key_anihortes_settings_north,
        .north_east = &key_anihortes_settings_north_east,
        .east       = &key_anihortes_settings_east,
        .south_east = &key_anihortes_settings_south_east,
        .south      = &key_anihortes_settings_south,
        .south_west = &key_anihortes_settings_south_west,
        .west       = &key_anihortes_settings_west,
        .north_west = &key_anihortes_settings_north_west,
        .long_tap   = &key_anihortes_settings_long_tap,
    },
    {"", "", 30.0, Pad},
    {"", "", 0.0, EndRow},

    {"", "", 70.0, Pad},
    {"h", "H", 1.0, Code, KEY_H, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_h_north,
        .north_east = &key_anihortes_h_north_east,
        .east       = &key_anihortes_h_east,
        .south_east = &key_anihortes_h_south_east,
        .south      = &key_anihortes_h_south,
        .south_west = &key_anihortes_h_south_west,
        .west       = &key_anihortes_h_west,
        .north_west = &key_anihortes_h_north_west,
        .long_tap   = &key_anihortes_h_long_tap,
    },
    {"o", "O", 1.0, Code, KEY_O, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_o_north,
        .north_east = &key_anihortes_o_north_east,
        .east       = &key_anihortes_o_east,
        .south_east = &key_anihortes_o_south_east,
        .south      = &key_anihortes_o_south,
        .south_west = &key_anihortes_o_south_west,
        .west       = &key_anihortes_o_west,
        .north_west = &key_anihortes_o_north_west,
        .long_tap   = &key_anihortes_o_long_tap,
    },
    {"r", "R", 1.0, Code, KEY_R, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_r_north,
        .north_east = &key_anihortes_r_north_east,
        .east       = &key_anihortes_r_east,
        .south_east = &key_anihortes_r_south_east,
        .south      = &key_anihortes_r_south,
        .south_west = &key_anihortes_r_south_west,
        .west       = &key_anihortes_r_west,
        .north_west = &key_anihortes_r_north_west,
        .long_tap   = &key_anihortes_r_long_tap,
    },
    {"abc", "abc", -2.0, Layout, 0, &layouts[Anihortes], .scheme = 0, .shape = OneSquare,
        .north      = &key_anihortes_navigation_north,
        .north_east = &key_anihortes_navigation_north_east,
        .east       = &key_anihortes_navigation_east,
        .south_east = &key_anihortes_navigation_south_east,
        .south      = &key_anihortes_navigation_south,
        .south_west = &key_anihortes_navigation_south_west,
        .west       = &key_anihortes_navigation_west,
        .north_west = &key_anihortes_navigation_north_west,
        .long_tap   = &key_anihortes_navigation_long_tap,
    },
    {"", "", 30.0, Pad},
    {"", "", 0.0, EndRow},

    {"", "", 70.0, Pad},
    {"t", "T", 1.0, Code, KEY_T, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_t_north,
        .north_east = &key_anihortes_t_north_east,
        .east       = &key_anihortes_t_east,
        .south_east = &key_anihortes_t_south_east,
        .south      = &key_anihortes_t_south,
        .south_west = &key_anihortes_t_south_west,
        .west       = &key_anihortes_t_west,
        .north_west = &key_anihortes_t_north_west,
        .long_tap   = &key_anihortes_t_long_tap,
    },
    {"e", "E", 1.0, Code, KEY_E, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_e_north,
        .north_east = &key_anihortes_e_north_east,
        .east       = &key_anihortes_e_east,
        .south_east = &key_anihortes_e_south_east,
        .south      = &key_anihortes_e_south,
        .south_west = &key_anihortes_e_south_west,
        .west       = &key_anihortes_e_west,
        .north_west = &key_anihortes_e_north_west,
        .long_tap   = &key_anihortes_e_long_tap,
    },
    {"s", "S", 1.0, Code, KEY_S, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_s_north,
        .north_east = &key_anihortes_s_north_east,
        .east       = &key_anihortes_s_east,
        .south_east = &key_anihortes_s_south_east,
        .south      = &key_anihortes_s_south,
        .south_west = &key_anihortes_s_south_west,
        .west       = &key_anihortes_s_west,
        .north_west = &key_anihortes_s_north_west,
        .long_tap   = &key_anihortes_s_long_tap,
    },
    {"⌫", "⌫", -2.0, Code, KEY_BACKSPACE, .scheme = 0, .shape = OneSquare,
        .north      = &key_anihortes_backspace_north,
        .north_east = &key_anihortes_backspace_north_east,
        .east       = &key_anihortes_backspace_east,
        .south_east = &key_anihortes_backspace_south_east,
        .south      = &key_anihortes_backspace_south,
        .south_west = &key_anihortes_backspace_south_west,
        .west       = &key_anihortes_backspace_west,
        .north_west = &key_anihortes_backspace_north_west,
        .long_tap   = &key_anihortes_backspace_long_tap,
    },
    {"", "", 30.0, Pad},
    {"", "", 0.0, EndRow},

    {"", "", 70.0, Pad},
    {"", "", 1.0, Code, KEY_SPACE, .scheme = 0, .shape = ThreeSquares,
        .north      = &key_anihortes_space_north,
        .north_east = &key_anihortes_space_north_east,
        .east       = &key_anihortes_space_east,
        .south_east = &key_anihortes_space_south_east,
        .south      = &key_anihortes_space_south,
        .south_west = &key_anihortes_space_south_west,
        .west       = &key_anihortes_space_west,
        .north_west = &key_anihortes_space_north_west,
        .long_tap   = &key_anihortes_space_long_tap,
    },
    {"↵", "↵", -2.0, Code, KEY_ENTER, .scheme = 0, .shape = OneSquare},
    {"", "", 30.0, Pad},

    /* end of layout */
    {"", "", 0.0, Last},
};

static struct key keys_numpad[] = {
    {"", "", 70.0, Pad},
    {"1", "1", -2.0, Code, KEY_1, .scheme = 1, .shape = OneSquare,
        //.north      = &key_anihortes_a_north,
        //.north_east = &key_anihortes_a_north_east,
        .east       = &key_anihortes_a_east,
        //.south_east = &key_anihortes_a_south_east,
        //.south      = &key_anihortes_a_south,
        .south_west = &key_anihortes_a_south_west,
        //.west       = &key_anihortes_a_west,
        //.north_west = &key_anihortes_a_north_west,
        //.long_tap   = &key_anihortes_a_long_tap,
    },
    {"2", "2", -2.0, Code, KEY_2, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_n_north,
        .north_east = &key_anihortes_n_north_east,
        .east       = &key_anihortes_n_east,
        .south_east = &key_anihortes_n_south_east,
        //.south      = &key_anihortes_n_south,
        .south_west = &key_anihortes_n_south_west,
        .west       = &key_anihortes_n_west,
        .north_west = &key_anihortes_n_north_west,
        .long_tap   = &key_anihortes_n_long_tap,
    },
    {"3", "3", -2.0, Code, KEY_3, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_i_north,
        .north_east = &key_anihortes_i_north_east,
        //.east       = &key_anihortes_i_east,
        .south_east = &key_anihortes_i_south_east,
        .south      = &key_anihortes_i_south,
        //.south_west = &key_anihortes_i_south_west,
        .west       = &key_anihortes_i_west,
        //.north_west = &key_anihortes_i_north_west,
        .long_tap   = &key_anihortes_i_long_tap,
    },
    {"☝", "☝", -2.0, Code, KEY_X, .scheme = 0, .shape = OneSquare, // TODO: Hide Keyboard?
        .north      = &key_anihortes_settings_north,
        .north_east = &key_anihortes_settings_north_east,
        .east       = &key_anihortes_settings_east,
        .south_east = &key_anihortes_settings_south_east,
        .south      = &key_anihortes_settings_south,
        .south_west = &key_anihortes_settings_south_west,
        .west       = &key_anihortes_settings_west,
        .north_west = &key_anihortes_settings_north_west,
        .long_tap   = &key_anihortes_settings_long_tap,
    },
    {"", "", 30.0, Pad},
    {"", "", 0.0, EndRow},

    {"", "", 70.0, Pad},
    {"4", "4", 1.0, Code, KEY_4, .scheme = 1, .shape = OneSquare,
        //.north      = &key_anihortes_h_north,
        .north_east = &key_anihortes_h_north_east,
        //.east       = &key_anihortes_h_east,
        .south_east = &key_anihortes_h_south_east,
        //.south      = &key_anihortes_h_south,
        .south_west = &key_anihortes_h_south_west,
        .west       = &key_anihortes_h_west,
        .north_west = &key_anihortes_h_north_west,
        .long_tap   = &key_anihortes_h_long_tap,
    },
    {"5", "5", 1.0, Code, KEY_5, .scheme = 1, .shape = OneSquare,
        //.north      = &key_anihortes_o_north,
        //.north_east = &key_anihortes_o_north_east,
        //.east       = &key_anihortes_o_east,
        //.south_east = &key_anihortes_o_south_east,
        //.south      = &key_anihortes_o_south,
        //.south_west = &key_anihortes_o_south_west,
        //.west       = &key_anihortes_o_west,
        //.north_west = &key_anihortes_o_north_west,
        .long_tap   = &key_anihortes_o_long_tap,
    },
    {"6", "6", 1.0, Code, KEY_6, .scheme = 1, .shape = OneSquare,
        //.north      = &key_anihortes_r_north,
        .north_east = &key_anihortes_r_north_east,
        .east       = &key_anihortes_r_east,
        .south_east = &key_anihortes_r_south_east,
        //.south      = &key_anihortes_r_south,
        .south_west = &key_anihortes_r_south_west,
        //.west       = &key_anihortes_r_west,
        .north_west = &key_anihortes_r_north_west,
        .long_tap   = &key_anihortes_r_long_tap,
    },
    {"abc", "abc", -2.0, Layout, 0, &layouts[Anihortes], .scheme = 0, .shape = OneSquare,
        .north      = &key_anihortes_navigation_north,
        .north_east = &key_anihortes_navigation_north_east,
        .east       = &key_anihortes_navigation_east,
        .south_east = &key_anihortes_navigation_south_east,
        .south      = &key_anihortes_navigation_south,
        .south_west = &key_anihortes_navigation_south_west,
        .west       = &key_anihortes_navigation_west,
        .north_west = &key_anihortes_navigation_north_west,
        .long_tap   = &key_anihortes_navigation_long_tap,
    },
    {"", "", 30.0, Pad},
    {"", "", 0.0, EndRow},

    {"", "", 70.0, Pad},
    {"7", "7", 1.0, Code, KEY_7, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_t_north,
        //.north_east = &key_anihortes_t_north_east,
        .east       = &key_anihortes_t_east,
        .south_east = &key_anihortes_t_south_east,
        .south      = &key_anihortes_t_south,
        .south_west = &key_anihortes_t_south_west,
        .west       = &key_anihortes_t_west,
        .north_west = &key_anihortes_t_north_west,
        .long_tap   = &key_anihortes_t_long_tap,
    },
    {"8", "8", 1.0, Code, KEY_8, .scheme = 1, .shape = OneSquare,
        //.north      = &key_anihortes_e_north,
        .north_east = &key_anihortes_e_north_east,
        //.east       = &key_anihortes_e_east,
        .south_east = &key_anihortes_e_south_east,
        .south      = &key_anihortes_e_south,
        .south_west = &key_anihortes_e_south_west,
        //.west       = &key_anihortes_e_west,
        .north_west = &key_anihortes_e_north_west,
        .long_tap   = &key_anihortes_e_long_tap,
    },
    {"9", "9", 1.0, Code, KEY_9, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_s_north,
        .north_east = &key_anihortes_s_north_east,
        .east       = &key_anihortes_s_east,
        .south_east = &key_anihortes_s_south_east,
        .south      = &key_anihortes_s_south,
        .south_west = &key_anihortes_s_south_west,
        .west       = &key_anihortes_s_west,
        //.north_west = &key_anihortes_s_north_west,
        .long_tap   = &key_anihortes_s_long_tap,
    },
    {"⌫", "⌫", -2.0, Code, KEY_BACKSPACE, .scheme = 0, .shape = OneSquare},
    {"", "", 30.0, Pad},
    {"", "", 0.0, EndRow},

    {"", "", 70.0, Pad},
    {"0", "0", 1.0, Code, KEY_0, .scheme = 1, .shape = TwoSquares},
    {"", "", 1.0, Code, KEY_SPACE, .scheme = 0, .shape = OneSquare,
        .north      = &key_anihortes_space_north,
        .north_east = &key_anihortes_space_north_east,
        .east       = &key_anihortes_space_east,
        .south_east = &key_anihortes_space_south_east,
        .south      = &key_anihortes_space_south,
        .south_west = &key_anihortes_space_south_west,
        .west       = &key_anihortes_space_west,
        .north_west = &key_anihortes_space_north_west,
        .long_tap   = &key_anihortes_space_long_tap,
    },
    {"↵", "↵", -2.0, Code, KEY_ENTER, .scheme = 0, .shape = OneSquare},
    {"", "", 30.0, Pad},

    /* end of layout */
    {"", "", 0.0, Last},
};

static struct key keys_navigation[] = {
    {"", "", 70.0, Pad},
    {"1", "1", -2.0, Code, KEY_1, 0, Super, .scheme = 0, .shape = OneSquare,
    },
    {"2", "2", -2.0, Code, KEY_2, 0, Super, .scheme = 0, .shape = OneSquare,
    },
    {"3", "3", -2.0, Code, KEY_3, 0, Super, .scheme = 0, .shape = OneSquare,
    },
    {"☝", "☝", -2.0, BackLayer, .scheme = 1, .shape = OneSquare,
    },
    {"", "", 30.0, Pad},
    {"", "", 0.0, EndRow},

    {"", "", 70.0, Pad},
    {"4", "4", 1.0, Code, KEY_4, 0, Super, .scheme = 0, .shape = OneSquare,
    },
    {"5", "5", 1.0, Code, KEY_5, 0, Super, .scheme = 0, .shape = OneSquare,
    },
    {"6", "6", 1.0, Code, KEY_6, 0, Super, .scheme = 0, .shape = OneSquare,
    },
    {"abc", "abc", -2.0, Layout, 0, &layouts[Anihortes], .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_navigation_north,
        .north_east = &key_anihortes_navigation_north_east,
        .east       = &key_anihortes_navigation_east,
        .south_east = &key_anihortes_navigation_south_east,
        .south      = &key_anihortes_navigation_south,
        .south_west = &key_anihortes_navigation_south_west,
        .west       = &key_anihortes_navigation_west,
        .north_west = &key_anihortes_navigation_north_west,
        .long_tap   = &key_anihortes_navigation_long_tap,
    },
    {"", "", 30.0, Pad},
    {"", "", 0.0, EndRow},

    {"", "", 70.0, Pad},
    {"7", "7", 1.0, Code, KEY_7, 0, Super, .scheme = 0, .shape = OneSquare,
    },
    {"8", "8", 1.0, Code, KEY_8, 0, Super, .scheme = 0, .shape = OneSquare,
    },
    {"9", "9", 1.0, Code, KEY_9, 0, Super, .scheme = 0, .shape = OneSquare,
    },
    {"⌫", "⌫", -2.0, Code, KEY_BACKSPACE, .scheme = 1, .shape = OneSquare},
    {"", "", 30.0, Pad},
    {"", "", 0.0, EndRow},

    {"", "", 70.0, Pad},
    {"0", "0", 1.0, Code, KEY_0, 0, Super, .scheme = 0, .shape = TwoSquares},
    {"", "", 1.0, Code, KEY_SPACE, .scheme = 1, .shape = OneSquare,
        .north      = &key_anihortes_space_north,
        .north_east = &key_anihortes_space_north_east,
        .east       = &key_anihortes_space_east,
        .south_east = &key_anihortes_space_south_east,
        .south      = &key_anihortes_space_south,
        .south_west = &key_anihortes_space_south_west,
        .west       = &key_anihortes_space_west,
        .north_west = &key_anihortes_space_north_west,
        .long_tap   = &key_anihortes_space_long_tap,
    },
    {"↵", "↵", -2.0, Code, KEY_ENTER, .scheme = 1, .shape = OneSquare},
    {"", "", 30.0, Pad},

    /* end of layout */
    {"", "", 0.0, Last},
};

static struct key keys_index[] = {
    {"abc", "abc", 1.0, Layout, 0, &layouts[Anihortes], .scheme = 1},
    {"123", "123", 1.0, Layout, 0, &layouts[NumPad], .scheme = 1},
    {"Nav", "Nav", 1.0, Layout, 0, &layouts[Navigation], .scheme = 1},
    {"", "", 0.0, Last},
};
