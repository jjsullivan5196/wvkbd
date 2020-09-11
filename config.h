/* constants */

/* how tall the keyboard should be */
#define KBD_PIXEL_HEIGHT 155

/* if your layout leaves an empty margin, increase this to fix it */
#define KBD_PIXEL_OVERSCAN_WIDTH 0

/* rows for each layout */
#define KBD_ROWS 4

/* columns for each layout (maximum keys per row) */
#define KBD_COLS 10

/* spacing between keys */
#define KBD_KEY_BORDER 1

#include "keyboard.h"

/* font (see `man fonts-conf` for instructions) */
static const char *fc_font_pattern =
  "monospace:size=14:antialias=true:hinting=true";

/* layout declarations */
enum layout_names {
	Basic = 0,
	Special,
	NumLayouts,
};

static struct key keys_basic[], keys_special[];

static struct layout layouts[NumLayouts] = {
  [Basic] = {keys_basic},
  [Special] = {keys_special},
};

/* keyboard settings */
static struct kbd keyboard = {
  /* default layout */
  .layout = &layouts[Basic],
  .scheme =
    {
      /* colors */
      .bg = {.bgra = {15, 15, 15, 225}},
      .fg = {.bgra = {45, 45, 45, 225}},
      .high = {.bgra = {100, 100, 100, 225}},
      .text = {.color = UINT32_MAX},
    },
};

/* key layouts
 *
 * define keys like:
 *
 *  `{
 *     "label",
 *     "SHIFT_LABEL",
 *     1,
 *     [Code, Mod, Layout, Last],
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
static struct key keys_basic[] = {
  {"q", "Q", 1, Code, KEY_Q},
  {"w", "W", 1, Code, KEY_W},
  {"e", "E", 1, Code, KEY_E},
  {"r", "R", 1, Code, KEY_R},
  {"t", "T", 1, Code, KEY_T},
  {"y", "Y", 1, Code, KEY_Y},
  {"u", "U", 1, Code, KEY_U},
  {"i", "I", 1, Code, KEY_I},
  {"o", "O", 1, Code, KEY_O},
  {"p", "P", 1, Code, KEY_P},

  {"a", "A", 1, Code, KEY_A},
  {"s", "S", 1, Code, KEY_S},
  {"d", "D", 1, Code, KEY_D},
  {"f", "F", 1, Code, KEY_F},
  {"g", "G", 1, Code, KEY_G},
  {"h", "H", 1, Code, KEY_H},
  {"j", "J", 1, Code, KEY_J},
  {"k", "K", 1, Code, KEY_K},
  {"l", "L", 1, Code, KEY_L},
  {";", ":", 1, Code, KEY_SEMICOLON},

  {"z", "Z", 1, Code, KEY_Z},
  {"x", "X", 1, Code, KEY_X},
  {"c", "C", 1, Code, KEY_C},
  {"v", "V", 1, Code, KEY_V},
  {"b", "B", 1, Code, KEY_B},
  {"n", "N", 1, Code, KEY_N},
  {"m", "M", 1, Code, KEY_M},
  {"Tab", "Tab", 1, Code, KEY_TAB},
  {"Bk", "Bk", 1, Code, KEY_BACKSPACE},
  {"Ret", "Ret", 1, Code, KEY_ENTER},

  {"Sft", "Sft", 1, Mod, Shift},
  {"Sp", "Sp", 1, Mod, Super},
  {"Alt", "Alt", 1, Mod, AltGr},
  {"", "", 3, Code, KEY_SPACE},
  {"Esc", "Esc", 1, Code, KEY_ESC},
  {"Ctl", "Ctl", 1, Mod, Ctrl},
  {"Sm", "Sm", 2, Layout, 0, &layouts[Special]},

  /* end of layout */
  {"", "", 0, Last},
};

static struct key keys_special[] = {
  {"1", "!", 1, Code, KEY_1},
  {"2", "@", 1, Code, KEY_2},
  {"3", "#", 1, Code, KEY_3},
  {"4", "$", 1, Code, KEY_4},
  {"5", "%", 1, Code, KEY_5},
  {"6", "^", 1, Code, KEY_6},
  {"7", "&", 1, Code, KEY_7},
  {"8", "*", 1, Code, KEY_8},
  {"9", "(", 1, Code, KEY_9},
  {"0", ")", 1, Code, KEY_0},

  {"'", "\"", 1, Code, KEY_APOSTROPHE},
  {"`", "~", 1, Code, KEY_GRAVE},
  {"-", "_", 1, Code, KEY_MINUS},
  {"=", "+", 1, Code, KEY_EQUAL},
  {"[", "{", 1, Code, KEY_LEFTBRACE},
  {"]", "}", 1, Code, KEY_RIGHTBRACE},
  {",", "<", 1, Code, KEY_COMMA},
  {".", ">", 1, Code, KEY_DOT},
  {"/", "?", 1, Code, KEY_SLASH},
  {"\\", "|", 1, Code, KEY_BACKSLASH},

  {"↑", "↑", 1, Code, KEY_UP},
  {"↓", "↓", 1, Code, KEY_DOWN},
  {"←", "←", 1, Code, KEY_LEFT},
  {"→", "→", 1, Code, KEY_RIGHT},
  {"Sp", "Sp", 1, Mod, Super},
  {"Alt", "Alt", 1, Mod, AltGr},
  {"Ctl", "Ctl", 1, Mod, Ctrl},
  {"Sft", "Sft", 1, Mod, Shift},

  {"Ret", "Ret", 1, Code, KEY_ENTER},
  {"Bk", "Bk", 1, Code, KEY_BACKSPACE},
  {"", "", 3, Code, KEY_SPACE},
  {"Bsc", "Bsc", 2, Layout, 0, &layouts[Basic]},

  /* end of layout */
  {"", "", 0, Last},
};
