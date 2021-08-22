/* constants */

/* how tall the keyboard should be */
#define KBD_PIXEL_HEIGHT 240

/* if your layout leaves an empty margin, increase this to fix it */
#define KBD_PIXEL_OVERSCAN_WIDTH 5

/* Maximum number of keys */
#define KBD_POINTS 66

/* spacing between keys */
#define KBD_KEY_BORDER 2

#include "keyboard.h"

/* font (see `man fonts-conf` for instructions) */
static const char *fc_font_pattern =
  "FiraMono Nerd Font:size=16:antialias=true:hinting=true";

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
static struct key keys_basic[] = {
  {"Esc", "Esc", 1.0, Code, KEY_ESC},
  {"Tab", "Tab", 1.0, Code, KEY_TAB},
  {"↑", "↑", 1.0, Code, KEY_UP},
  {"↓", "↓", 1.0, Code, KEY_DOWN},
  {"←", "←", 1.0, Code, KEY_LEFT},
  {"→", "→", 1.0, Code, KEY_RIGHT},
  {"'", "\"", 1.0, Code, KEY_APOSTROPHE},
  {"/", "?", 1.0, Code, KEY_SLASH},
  {";", ":", 1.0, Code, KEY_SEMICOLON},
  {"`", "~", 1.0, Code, KEY_GRAVE},
  {"", "", 0.0, EndRow},

  {"1", "!", 1.0, Code, KEY_1},
  {"2", "@", 1.0, Code, KEY_2},
  {"3", "#", 1.0, Code, KEY_3},
  {"4", "$", 1.0, Code, KEY_4},
  {"5", "%", 1.0, Code, KEY_5},
  {"6", "^", 1.0, Code, KEY_6},
  {"7", "&", 1.0, Code, KEY_7},
  {"8", "*", 1.0, Code, KEY_8},
  {"9", "(", 1.0, Code, KEY_9},
  {"0", ")", 1.0, Code, KEY_0},
  {"-", "_", 1.0, Code, KEY_MINUS},
  {"=", "+", 1.0, Code, KEY_EQUAL},
  {"", "", 0.0, EndRow},

  {"", "", 0.5, Pad},
  {"q", "Q", 1.0, Code, KEY_Q},
  {"w", "W", 1.0, Code, KEY_W},
  {"e", "E", 1.0, Code, KEY_E},
  {"r", "R", 1.0, Code, KEY_R},
  {"t", "T", 1.0, Code, KEY_T},
  {"y", "Y", 1.0, Code, KEY_Y},
  {"u", "U", 1.0, Code, KEY_U},
  {"i", "I", 1.0, Code, KEY_I},
  {"o", "O", 1.0, Code, KEY_O},
  {"p", "P", 1.0, Code, KEY_P},
  {"", "", 0.0, EndRow},

  {"Ct", "Ct", 1.0, Mod, Ctrl},
  {"a", "A", 1.0, Code, KEY_A},
  {"s", "S", 1.0, Code, KEY_S},
  {"d", "D", 1.0, Code, KEY_D},
  {"f", "F", 1.0, Code, KEY_F},
  {"g", "G", 1.0, Code, KEY_G},
  {"h", "H", 1.0, Code, KEY_H},
  {"j", "J", 1.0, Code, KEY_J},
  {"k", "K", 1.0, Code, KEY_K},
  {"l", "L", 1.0, Code, KEY_L},
  {"", "", 0.5, Pad},
  {"", "", 0.0, EndRow},

  {"⇧", "⇧", 1.5, Mod, Shift},
  {"z", "Z", 1.0, Code, KEY_Z},
  {"x", "X", 1.0, Code, KEY_X},
  {"c", "C", 1.0, Code, KEY_C},
  {"v", "V", 1.0, Code, KEY_V},
  {"b", "B", 1.0, Code, KEY_B},
  {"n", "N", 1.0, Code, KEY_N},
  {"m", "M", 1.0, Code, KEY_M},
  {"⌫", "⌫", 1.5, Code, KEY_BACKSPACE},
  {"", "", 0.0, EndRow},

  {"Sym", "Sym", 1.0, Layout, 0, &layouts[Special]},
  {"Alt", "Alt", 1.0, Mod, AltGr},
  {",", "<", 1.0, Code, KEY_COMMA},
  {"", "", 4.0, Code, KEY_SPACE},
  {".", ">", 1.0, Code, KEY_DOT},
  {"Entr", "Entr", 2.0, Code, KEY_ENTER},


  /* end of layout */
  {"", "", 0.0, Last},
};

static struct key keys_special[] = {
  {"Esc", "Esc", 1.0, Code, KEY_ESC},
  {"Tab", "Tab", 1.0, Code, KEY_TAB},
  {"↑", "↑", 1.0, Code, KEY_UP},
  {"↓", "↓", 1.0, Code, KEY_DOWN},
  {"←", "←", 1.0, Code, KEY_LEFT},
  {"→", "→", 1.0, Code, KEY_RIGHT},
  {"⇈", "⇈", 1.0, Code, KEY_PAGEUP},
  {"⇊", "⇊", 1.0, Code, KEY_PAGEDOWN},
  {"⇤", "⇤", 1.0, Code, KEY_HOME},
  {"⇥", "⇥", 1.0, Code, KEY_END},
  {"", "", 0.0, EndRow},

  {"1", "!", 1.0, Code, KEY_1},
  {"2", "@", 1.0, Code, KEY_2},
  {"3", "#", 1.0, Code, KEY_3},
  {"4", "$", 1.0, Code, KEY_4},
  {"5", "%", 1.0, Code, KEY_5},
  {"6", "^", 1.0, Code, KEY_6},
  {"7", "&", 1.0, Code, KEY_7},
  {"8", "*", 1.0, Code, KEY_8},
  {"9", "(", 1.0, Code, KEY_9},
  {"0", ")", 1.0, Code, KEY_0},
  {"", "", 0.0, EndRow},

  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 0.0, EndRow},

  {"Ct", "Ct", 2.0, Mod, Ctrl},
  {"`", "~", 1.0, Code, KEY_GRAVE},
  {"'", "\"", 1.0, Code, KEY_APOSTROPHE},
  {"-", "_", 1.0, Code, KEY_MINUS},
  {"=", "+", 1.0, Code, KEY_EQUAL},
  {"[", "{", 1.0, Code, KEY_LEFTBRACE},
  {"]", "}", 1.0, Code, KEY_RIGHTBRACE},
  {"\\", "|", 1.0, Code, KEY_BACKSLASH},
  {"Del", "Del", 1.0, Code, KEY_DELETE},
  {"", "", 0.0, EndRow},

  {"⇧", "⇧", 2.0, Mod, Shift},
  {";", ":", 1.0, Code, KEY_SEMICOLON},
  {"/", "?", 1.0, Code, KEY_SLASH},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"", "", 1.0, Code, KEY_SPACE},
  {"⌫", "⌫", 1.0, Code, KEY_BACKSPACE},
  {"", "", 0.0, EndRow},

  {"Abc", "Abc", 1.0, Layout, 0, &layouts[Basic]},
  {"Alt", "Alt", 1.0, Mod, AltGr},
  {",", "<", 1.0, Code, KEY_COMMA},
  {"", "", 4.0, Code, KEY_SPACE},
  {".", ">", 1.0, Code, KEY_DOT},
  {"Entr", "Entr", 2.0, Code, KEY_ENTER},

  /* end of layout */
  {"", "", 0.0, Last},
};
