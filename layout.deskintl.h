/* constants */
/* how tall the keyboard should be by default (can be overriden) */
#define KBD_PIXEL_HEIGHT 400

/* how tall the keyboard should be by default (can be overriden) */
#define KBD_PIXEL_LANDSCAPE_HEIGHT 400

/* spacing around each key */
#define KBD_KEY_BORDER 2

/* layout declarations */
enum layout_id {
	Full = 0,
	Special,
	ComposeA,
	ComposeE,
	ComposeY,
	ComposeU,
	ComposeI,
	ComposeO,
	ComposeW,
	ComposeR,
	ComposeT,
	ComposeP,
	ComposeS,
	ComposeD,
	ComposeF,
	ComposeG,
	ComposeH,
	ComposeJ,
	ComposeK,
	ComposeL,
	ComposeZ,
	ComposeX,
	ComposeC,
	ComposeV,
	ComposeB,
	ComposeN,
	ComposeM,
	ComposeMath,
	ComposePunctuation,
	ComposeBracket,
	Index,
	NumLayouts,
};

static struct key keys_full[], keys_special[],
  keys_compose_a[],
  keys_compose_e[], keys_compose_y[], keys_compose_u[], keys_compose_i[],
  keys_compose_o[], keys_compose_w[], keys_compose_r[], keys_compose_t[],
  keys_compose_p[], keys_compose_s[], keys_compose_d[], keys_compose_f[],
  keys_compose_g[], keys_compose_h[], keys_compose_j[], keys_compose_k[],
  keys_compose_l[], keys_compose_z[], keys_compose_x[], keys_compose_c[],
  keys_compose_v[], keys_compose_b[], keys_compose_n[], keys_compose_m[],
  keys_compose_math[], keys_compose_punctuation[], keys_compose_bracket[],
  keys_index[];

static struct layout layouts[NumLayouts] = {
  [Full] = {keys_full, "latin", "full", true}, // second parameter is the keymap name
                                         // third parameter is the layout name
										 // last parameter indicates if it's an alphabetical/primary layout
  [Special] = {keys_special, "latin", "special", false},
  [ComposeA] = {keys_compose_a, "latin"},
  [ComposeE] = {keys_compose_e, "latin"},
  [ComposeY] = {keys_compose_y, "latin"},
  [ComposeU] = {keys_compose_u, "latin"},
  [ComposeI] = {keys_compose_i, "latin"},
  [ComposeO] = {keys_compose_o, "latin"},
  [ComposeW] = {keys_compose_w, "latin"},
  [ComposeR] = {keys_compose_r, "latin"},
  [ComposeT] = {keys_compose_t, "latin"},
  [ComposeP] = {keys_compose_p, "latin"},
  [ComposeS] = {keys_compose_s, "latin"},
  [ComposeD] = {keys_compose_d, "latin"},
  [ComposeF] = {keys_compose_f, "latin"},
  [ComposeG] = {keys_compose_g, "latin"},
  [ComposeH] = {keys_compose_h, "latin"},
  [ComposeJ] = {keys_compose_j, "latin"},
  [ComposeK] = {keys_compose_k, "latin"},
  [ComposeL] = {keys_compose_l, "latin"},
  [ComposeZ] = {keys_compose_z, "latin"},
  [ComposeX] = {keys_compose_x, "latin"},
  [ComposeC] = {keys_compose_c, "latin"},
  [ComposeV] = {keys_compose_v, "latin"},
  [ComposeB] = {keys_compose_b, "latin"},
  [ComposeN] = {keys_compose_n, "latin"},
  [ComposeM] = {keys_compose_m, "latin"},
  [ComposeMath] = {keys_compose_math, "latin"},
  [ComposePunctuation] = {keys_compose_punctuation, "latin"},
  [ComposeBracket] = {keys_compose_bracket, "latin"},

  [Index] = {keys_index,"latin","index", false},
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
static struct key keys_full[] = {
  {"Esc", "Esc", 1.25, Code, KEY_ESC, .scheme = 1},
  {"F1", "F1", 1.0, Code, KEY_F1, .scheme = 1},
  {"F2", "F2", 1.0, Code, KEY_F2, .scheme = 1},
  {"F3", "F3", 1.0, Code, KEY_F3, .scheme = 1},
  {"F4", "F4", 1.0, Code, KEY_F4, .scheme = 1},
  {"F5", "F5", 1.0, Code, KEY_F5, .scheme = 1},
  {"F6", "F6", 1.0, Code, KEY_F6, .scheme = 1},
  {"F7", "F7", 1.0, Code, KEY_F7, .scheme = 1},
  {"F8", "F8", 1.0, Code, KEY_F8, .scheme = 1},
  {"F9", "F9", 1.0, Code, KEY_F9, .scheme = 1},
  {"F10", "F10", 1.0, Code, KEY_F10, .scheme = 1},
  {"F11", "F11", 1.0, Code, KEY_F11, .scheme = 1},
  {"F12", "F12", 1.0, Code, KEY_F12, .scheme = 1},
  {"Del", "Del", 1.25, Code, KEY_DELETE, .scheme = 1},
  {"", "", 0.0, EndRow},

  {"`", "~", 1.0, Code, KEY_GRAVE},
  {"1", "!", 1.0, Code, KEY_1},
  {"2", "@", 1.0, Code, KEY_2},
  {"3", "#", 1.0, Code, KEY_3},
  {"4", "$", 1.0, Code, KEY_4},
  {"5", "%", 1.0, Code, KEY_5},
  {"6", "^", 1.0, Code, KEY_6},
  {"7", "&", 1.0, Code, KEY_7},
  {"8", "*", 1.0, Code, KEY_8},
  {"9", "(", 1.0, Code, KEY_9, &layouts[ComposeBracket]},
  {"0", ")", 1.0, Code, KEY_0, &layouts[ComposeBracket]},
  {"-", "_", 1.0, Code, KEY_MINUS, &layouts[ComposeBracket]},
  {"=", "+", 1.0, Code, KEY_EQUAL, &layouts[ComposeBracket]},
  {"⌫", "⌫", 1.5, Code, KEY_BACKSPACE, .scheme = 1},
  {"", "", 0.0, EndRow},

  {"Tab", "Tab", 1.5, Code, KEY_TAB, .scheme = 1},
  {"q", "Q", 1.0, Code, KEY_Q},
  {"w", "W", 1.0, Code, KEY_W, &layouts[ComposeW]},
  {"e", "E", 1.0, Code, KEY_E, &layouts[ComposeE]},
  {"r", "R", 1.0, Code, KEY_R, &layouts[ComposeR]},
  {"t", "T", 1.0, Code, KEY_T, &layouts[ComposeT]},
  {"y", "Y", 1.0, Code, KEY_Y, &layouts[ComposeY]},
  {"u", "U", 1.0, Code, KEY_U, &layouts[ComposeU]},
  {"i", "I", 1.0, Code, KEY_I, &layouts[ComposeI]},
  {"o", "O", 1.0, Code, KEY_O, &layouts[ComposeO]},
  {"p", "P", 1.0, Code, KEY_P, &layouts[ComposeP]},
  {"[", "{", 1.0, Code, KEY_LEFTBRACE},
  {"]", "}", 1.0, Code, KEY_RIGHTBRACE},
  {"\\", "|", 1.0, Code, KEY_BACKSLASH},
  {"", "", 0.0, EndRow},

  {"Cmp", "Cmp", 1.0, Compose, .scheme = 1},
  {"Caps", "Caps", 1.0, Mod, CapsLock, .scheme = 1},
  {"a", "A", 1.0, Code, KEY_A, &layouts[ComposeA]},
  {"s", "S", 1.0, Code, KEY_S, &layouts[ComposeS]},
  {"d", "D", 1.0, Code, KEY_D, &layouts[ComposeD]},
  {"f", "F", 1.0, Code, KEY_F, &layouts[ComposeF]},
  {"g", "G", 1.0, Code, KEY_G, &layouts[ComposeG]},
  {"h", "H", 1.0, Code, KEY_H, &layouts[ComposeH]},
  {"j", "J", 1.0, Code, KEY_J, &layouts[ComposeJ]},
  {"k", "K", 1.0, Code, KEY_K, &layouts[ComposeK]},
  {"l", "L", 1.0, Code, KEY_L, &layouts[ComposeL]},
  {";", ":", 1.0, Code, KEY_SEMICOLON},
  {"'", "''", 1.0, Code, KEY_APOSTROPHE, &layouts[ComposeBracket]},
  {"Enter", "Enter", 1.5, Code, KEY_ENTER, .scheme = 1},
  {"", "", 0.0, EndRow},

  {"⇧", "⇫", 2.5, Mod, Shift, .scheme = 1},
  {"z", "Z", 1.0, Code, KEY_Z, &layouts[ComposeZ]},
  {"x", "X", 1.0, Code, KEY_X, &layouts[ComposeX]},
  {"c", "C", 1.0, Code, KEY_C, &layouts[ComposeC]},
  {"v", "V", 1.0, Code, KEY_V, &layouts[ComposeV]},
  {"b", "B", 1.0, Code, KEY_B, &layouts[ComposeB]},
  {"n", "N", 1.0, Code, KEY_N, &layouts[ComposeN]},
  {"m", "M", 1.0, Code, KEY_M, &layouts[ComposeM]},
  {",", "<", 1.0, Code, KEY_COMMA, &layouts[ComposeMath]},
  {".", ">", 1.0, Code, KEY_DOT, &layouts[ComposePunctuation]},
  {"/", "?", 1.0, Code, KEY_SLASH},
  {"↑", "↑", 1.0, Code, KEY_UP, .scheme = 1},
  {"⇧", "⇫", 1.0, Mod, Shift, .scheme = 1},
  {"", "", 0.0, EndRow},

  {"⌨͕", "⌨͔", 1.5, NextLayer, .scheme = 1},
  {"Ctr", "Ctr", 1.0, Mod, Ctrl, .scheme = 1},
  {"Sup", "Sup", 1.0, Mod, Super, .scheme = 1},
  {"Alt", "Alt", 1.0, Mod, Alt, .scheme = 1},
  {"", "", 5.0, Code, KEY_SPACE},
  {"AGr", "AGr", 1.0, Mod, AltGr, .scheme = 1},
  {"Ctr", "Ctr", 1.0, Mod, Ctrl, .scheme = 1},
  {"←", "←", 1.0, Code, KEY_LEFT, .scheme = 1},
  {"↓", "↓", 1.0, Code, KEY_DOWN, .scheme = 1},
  {"→", "→", 1.0, Code, KEY_RIGHT, .scheme = 1},
  /* end of layout */
  {"", "", 0.0, Last},
};

static struct key keys_special[] = {
  {"", "", 13.25, Pad},
  {"Ins", "Ins", 1.25, Code, KEY_INSERT, .scheme = 1},
  {"", "", 0.0, EndRow},

  {"", "", 14.5, Pad},
  {"", "", 0.0, EndRow},

  {"", "", 14.5, Pad},
  {"", "", 0.0, EndRow},

  {"", "", 14.5, Pad},
  {"", "", 0.0, EndRow},

  {"⇧", "⇫", 2.5, Mod, Shift, .scheme = 1},
  {"", "", 10.0, Pad},
  {"PgUp", "PgUp", 1.0, Code, KEY_PAGEUP, .scheme = 1},
  {"⇧", "⇫", 1.0, Mod, Shift, .scheme = 1},
  {"", "", 0.0, EndRow},

  {"⌨͕", "⌨͔", 0.75, NextLayer, .scheme = 1},
  {"Abc", "Abc", 0.75, BackLayer, .scheme = 1},

  {"Ctr", "Ctr", 1.0, Mod, Ctrl, .scheme = 1},
  {"Sup", "Sup", 1.0, Mod, Super, .scheme = 1},
  {"Alt", "Alt", 1.0, Mod, Alt, .scheme = 1},
  {"", "", 5.0, Pad},
  {"AGr", "AGr", 1.0, Mod, AltGr, .scheme = 1},
  {"Ctr", "Ctr", 1.0, Mod, Ctrl, .scheme = 1},
  {"Home", "Home", 1.0, Code, KEY_HOME, .scheme = 1},
  {"PgDn", "PgDn", 1.0, Code, KEY_PAGEDOWN, .scheme = 1},
  {"End", "End", 1.0, Code, KEY_END, .scheme = 1},
  /* end of layout */
  {"", "", 0.0, Last},
};

static struct key keys_compose_a[] = {
  {"à", "À", 1.0, Copy, 0x00E0, 0, 0x00C0},
  {"á", "Á", 1.0, Copy, 0x00E1, 0, 0x00C1},
  {"â", "Â", 1.0, Copy, 0x00E2, 0, 0x00C2},
  {"ã", "Ã", 1.0, Copy, 0x00E3, 0, 0x00C3},
  {"ä", "Ä", 1.0, Copy, 0x00E4, 0, 0x00C4},
  {"å", "Å", 1.0, Copy, 0x00E5, 0, 0x00C5},
  {"æ", "Æ", 1.0, Copy, 0x00E7, 0, 0x00C6},
  {"ā", "Ā", 1.0, Copy, 0x0101, 0, 0x0100},
  {"ă", "Ă", 1.0, Copy, 0x0103, 0, 0x0102},
  {"ą", "Ą", 1.0, Copy, 0x0105, 0, 0x0104},
  {"", "", 0.0, EndRow},
  {"α", "Α", 1.0, Copy, 0x03B1, 0, 0x0391},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_e[] = {
  {"è", "È", 1.0, Copy, 0x00E8, 0, 0x00C8},
  {"é", "É", 1.0, Copy, 0x00E9, 0, 0x00C9},
  {"ê", "Ê", 1.0, Copy, 0x00EA, 0, 0x00CA},
  {"ë", "Ë", 1.0, Copy, 0x00EB, 0, 0x00CB},
  {"ē", "Ē", 1.0, Copy, 0x0113, 0, 0x0112},
  {"ĕ", "Ĕ", 1.0, Copy, 0x0115, 0, 0x0114},
  {"ė", "Ė", 1.0, Copy, 0x0117, 0, 0x0116},
  {"ę", "Ę", 1.0, Copy, 0x0119, 0, 0x0118},
  {"ě", "Ě", 1.0, Copy, 0x011B, 0, 0x011A},
  {"", "", 1.0, Pad},
  {"", "", 0.0, EndRow},
  {"ε", "Ε", 1.0, Copy, 0x03B5, 0, 0x0395},
  {"ǝ", "Ə", 1.0, Copy, 0x0259, 0, 0x018F},
  {"", "", 8.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_y[] = {
  {"ý", "Ý", 1.0, Copy, 0x00FD, 0, 0x00DD},
  {"ÿ", "Ÿ", 1.0, Copy, 0x00FF, 0, 0x0178},
  {"ŷ", "Ŷ", 1.0, Copy, 0x0177, 0, 0x0176},
  {"", "", 7.0, Pad},
  {"", "", 0.0, EndRow},
  {"υ", "Υ", 1.0, Copy, 0x03C5, 0, 0x03A5},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_u[] = {
  {"ù", "Ù", 1.0, Copy, 0x00F9, 0, 0x00D9},
  {"ú", "Ú", 1.0, Copy, 0x00FA, 0, 0x00DA},
  {"û", "Û", 1.0, Copy, 0x00FB, 0, 0x00DB},
  {"ü", "Ü", 1.0, Copy, 0x00FC, 0, 0x00DC},
  {"ũ", "Ũ", 1.0, Copy, 0x0169, 0, 0x0168},
  {"ū", "Ū", 1.0, Copy, 0x016B, 0, 0x016A},
  {"ŭ", "Ŭ", 1.0, Copy, 0x016D, 0, 0x016C},
  {"ů", "Ů", 1.0, Copy, 0x016F, 0, 0x016E},
  {"ű", "Ű", 1.0, Copy, 0x0171, 0, 0x0170},
  {"ų", "Ų", 1.0, Copy, 0x0173, 0, 0x0172},
  {"", "", 0.0, EndRow},
  {"υ", "Υ", 1.0, Copy, 0x03C5, 0, 0x03A5},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_o[] = {
  {"ò", "Ò", 1.0, Copy, 0x00F2, 0, 0x00D2},
  {"ó", "Ó", 1.0, Copy, 0x00F3, 0, 0x00D3},
  {"ô", "Ô", 1.0, Copy, 0x00F4, 0, 0x00D4},
  {"õ", "Õ", 1.0, Copy, 0x00F5, 0, 0x00D5},
  {"ö", "Ö", 1.0, Copy, 0x00F6, 0, 0x00D6},
  {"ø", "Ø", 1.0, Copy, 0x00F8, 0, 0x00D8},
  {"ō", "Ō", 1.0, Copy, 0x014D, 0, 0x014C},
  {"ŏ", "Ŏ", 1.0, Copy, 0x014F, 0, 0x014E},
  {"ő", "Ő", 1.0, Copy, 0x0151, 0, 0x0150},
  {"œ", "Œ", 1.0, Copy, 0x0153, 0, 0x0152},
  {"", "", 0.0, EndRow},
  {"ο", "Ο", 1.0, Copy, 0x03BF, 0, 0x039F},
  {"ω", "Ο", 1.0, Copy, 0x03C9, 0, 0x03A9},
  {"", "", 8.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_i[] = {
  {"ì", "Ì", 1.0, Copy, 0x00EC, 0, 0x00CC},
  {"í", "Í", 1.0, Copy, 0x00ED, 0, 0x00CD},
  {"î", "Î", 1.0, Copy, 0x00EE, 0, 0x00CE},
  {"ï", "Ï", 1.0, Copy, 0x00EF, 0, 0x00CF},
  {"ĩ", "Ĩ", 1.0, Copy, 0x0129, 0, 0x0128},
  {"ī", "Ī", 1.0, Copy, 0x012B, 0, 0x012A},
  {"ĭ", "Ĭ", 1.0, Copy, 0x012D, 0, 0x012C},
  {"į", "Į", 1.0, Copy, 0x012F, 0, 0x012E},
  {"ı", "I", 1.0, Copy, 0x0131, 0, 0x0049},
  {"i", "İ", 1.0, Copy, 0x0069, 0, 0x0130},
  {"", "", 0.0, EndRow},
  {"ι", "Ι", 1.0, Copy, 0x03B9, 0, 0x0399},
  {"η", "Η", 1.0, Copy, 0x03B7, 0, 0x0397},
  {"", "", 8.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_index[] = {
  {"Full", "Full", 1.0, Layout, 0, &layouts[Full], .scheme = 1},
  {"Special", "Special", 1.0, Layout, 0, &layouts[Special], .scheme = 1},
  {"", "", 0.0, Last},
};

static struct key keys_compose_w[] = {
  {"ŵ", "Ŵ", 1.0, Copy, 0x0175, 0, 0x0174},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_r[] = {
  {"ŕ", "Ŕ", 1.0, Copy, 0x0155, 0, 0x0154},
  {"ŗ", "Ŗ", 1.0, Copy, 0x0157, 0, 0x0156},
  {"ř", "Ř", 1.0, Copy, 0x0159, 0, 0x0158},
  {"", "", 7.0, Pad},
  {"", "", 0.0, EndRow},
  {"ρ", "Ρ", 1.0, Copy, 0x03C1, 0, 0x03A1},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_t[] = {
  {"ț", "Ț", 1.0, Copy, 0x021B, 0, 0x021A},
  {"ť", "Ť", 1.0, Copy, 0x0165, 0, 0x0164},
  {"ŧ", "Ŧ", 1.0, Copy, 0x0167, 0, 0x0166},
  {"þ", "Þ", 1.0, Copy, 0x00FE, 0, 0x00DE},
  {"", "", 6.0, Pad},
  {"", "", 0.0, EndRow},
  {"τ", "Τ", 1.0, Copy, 0x03C4, 0, 0x03A4},
  {"θ", "Θ", 1.0, Copy, 0x03B8, 0, 0x0398},
  {"", "", 8.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_p[] = {
  {"π", "Π", 1.0, Copy, 0x03C0, 0, 0x03A0},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_s[] = {
  {"ś", "Ś", 1.0, Copy, 0x015B, 0, 0x015A},
  {"ŝ", "Ŝ", 1.0, Copy, 0x015D, 0, 0x015C},
  {"ş", "Ş", 1.0, Copy, 0x015F, 0, 0x015E},
  {"š", "Š", 1.0, Copy, 0x0161, 0, 0x0160},
  {"ß", "ẞ", 1.0, Copy, 0x00DF, 0, 0x1E9E},
  {"", "", 5.0, Pad},
  {"", "", 0.0, EndRow},
  {"σ", "Σ", 1.0, Copy, 0x03C3, 0, 0x03A3},
  {"ς", "Σ", 1.0, Copy, 0x03C2, 0, 0x03A3},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_d[] = {
  {"ð", "Ð", 1.0, Copy, 0x00F0, 0, 0x00D0},
  {"ď", "Ď", 1.0, Copy, 0x010F, 0, 0x010E},
  {"đ", "Đ", 1.0, Copy, 0x0111, 0, 0x0110},
  {"", "", 7.0, Pad},
  {"", "", 0.0, EndRow},
  {"δ", "Δ", 1.0, Copy, 0x03B4, 0, 0x0394},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_f[] = {
  {"φ", "Φ", 1.0, Copy, 0x03C6, 0, 0x03A6},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_g[] = {
  {"ĝ", "Ĝ", 1.0, Copy, 0x011D, 0, 0x011C},
  {"ğ", "Ğ", 1.0, Copy, 0x011F, 0, 0x011E},
  {"", "", 8.0, Pad},
  {"", "", 0.0, EndRow},
  {"γ", "Γ", 1.0, Copy, 0x03B3, 0, 0x0393},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_h[] = {
  {"ĥ", "Ĥ", 1.0, Copy, 0x0125, 0, 0x0124},
  {"ħ", "Ħ", 1.0, Copy, 0x0127, 0, 0x0126},
  {"", "", 8.0, Pad},
  {"", "", 0.0, EndRow},
  {"η", "Η", 1.0, Copy, 0x03B7, 0, 0x0397},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_j[] = {
  {"ĵ", "Ĵ", 1.0, Copy, 0x0135, 0, 0x0134},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"", "", 10.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_k[] = {
  {"ķ", "Ķ", 1.0, Copy, 0x0137, 0, 0x0136},
  {"ǩ", "Ǩ", 1.0, Copy, 0x01E9, 0, 0x01E8},
  {"", "", 8.0, Pad},
  {"", "", 0.0, EndRow},
  {"κ", "Κ", 1.0, Copy, 0x03BA, 0, 0x039A},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_l[] = {
  {"ľ", "Ľ", 1.0, Copy, 0x013E, 0, 0x013D},
  {"ŀ", "Ŀ", 1.0, Copy, 0x0140, 0, 0x013F},
  {"ł", "Ł", 1.0, Copy, 0x0142, 0, 0x0141},
  {"", "", 7.0, Pad},
  {"", "", 0.0, EndRow},
  {"λ", "Λ", 1.0, Copy, 0x03BB, 0, 0x039B},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_z[] = {
  {"ź", "Ź", 1.0, Copy, 0x017A, 0, 0x0179},
  {"ż", "Ż", 1.0, Copy, 0x017C, 0, 0x017B},
  {"ž", "Ž", 1.0, Copy, 0x017E, 0, 0x017D},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"ζ", "Ζ", 1.0, Copy, 0x03B6, 0, 0x0396},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_x[] = {
  {"χ", "Χ", 1.0, Copy, 0x03C7, 0, 0x03A7},
  {"ξ", "Ξ", 1.0, Copy, 0x03BE, 0, 0x039E},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_c[] = {
  {"ç", "Ç", 1.0, Copy, 0x00E7, 0, 0x00C7},
  {"ć", "Ć", 1.0, Copy, 0x0107, 0, 0x0106},
  {"ĉ", "Ĉ", 1.0, Copy, 0x0109, 0, 0x0108},
  {"ċ", "Ċ", 1.0, Copy, 0x010B, 0, 0x010A},
  {"č", "Č", 1.0, Copy, 0x010D, 0, 0x010C},
  {"", "", 5.0, Pad},
  {"", "", 0.0, EndRow},
  {"χ", "Χ", 1.0, Copy, 0x03C7, 0, 0x03A7},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_v[] = {
  {"", "", 0.0, EndRow},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_b[] = {
  {"β", "Β", 1.0, Copy, 0x03B2, 0, 0x0392},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_n[] = {
  {"ñ", "Ñ", 1.0, Copy, 0x00F1, 0, 0x00D1},
  {"ń", "Ń", 1.0, Copy, 0x0144, 0, 0x0143},
  {"ņ", "Ņ", 1.0, Copy, 0x0146, 0, 0x0145},
  {"ň", "Ň", 1.0, Copy, 0x0148, 0, 0x0147},
  {"ŋ", "Ŋ", 1.0, Copy, 0x014B, 0, 0x014A},
  {"", "", 5.0, Pad},
  {"", "", 0.0, EndRow},
  {"ν", "Ν", 1.0, Copy, 0x03BD, 0, 0x039D},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_m[] = {
  {"μ", "Μ", 1.0, Copy, 0x03BC, 0, 0x039C},
  {"", "", 9.0, Pad},
  {"", "", 0.0, EndRow},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 8.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_math[] = {
  {"", "", 0.0, EndRow},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"+", "+", 1, Code, KEY_EQUAL, 0, Shift},
  {"/", "/", 1, Code, KEY_SLASH},
  {"*", "*", 1, Code, KEY_8, 0, Shift},
  {"-", "-", 1, Code, KEY_MINUS},
  {"=", "=", 1, Code, KEY_EQUAL},
  {"_", "_", 1, Code, KEY_MINUS, 0, Shift},
  {"—", "—", 1, Copy, 0x2014, 0, 0x2014},
  {"", "", 1.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_punctuation[] = {
  {"", "", 0.0, EndRow},
  {"", "", 4.5, Pad},
  {".", ".", 1, Code, KEY_DOT},
  {"…", "…", 1, Copy, 0x2026, 0, 0x2026},
  {":", ":", 1, Code, KEY_SEMICOLON, 0, Shift},
  {";", ";", 1, Code, KEY_SEMICOLON, 0},
  {"⍽", "⍽", 1, Copy, 0x202F, 0, 0x00A0},
  {"", "", 0.5, Pad},
  {"", "", 0.0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"", "", 3, Pad},
  {"!", "!", 1, Code, KEY_1, 0, Shift},
  {"?", "?", 1, Code, KEY_DOT, 0, Shift},
  {"·", "·", 1, Copy, 0x2027, 0, 0x2027},
  {",", ",", 1, Code, KEY_COMMA},
  {"", "", 1.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"", "", 9, Pad},
  {"", "", 0.0, Last},
};

static struct key keys_compose_bracket[] = {
  {"", "", 0, EndRow},
  {"", "", 1.5, Pad},
  {"{", "{", 1, Code, KEY_LEFTBRACE, 0, Shift},
  {"}", "}", 1, Code, KEY_RIGHTBRACE, 0, Shift},
  {"[", "[", 1, Code, KEY_LEFTBRACE},
  {"]", "]", 1, Code, KEY_RIGHTBRACE},
  {"", "", 4.5, Pad},
  {"", "", 0, EndRow},
  {"⇧", "⇫", 1.5, Mod, Shift, .scheme = 1},
  {"<", "<", 1, Code, KEY_COMMA, 0, AltGr},
  {">", ">", 1, Code, KEY_SLASH, 0, Shift},
  {"`", "`", 1, Code, KEY_GRAVE},
  {"\"", "\"", 1, Code, KEY_APOSTROPHE, 0, Shift},
  {"'", "'", 1, Code, KEY_APOSTROPHE},
  {"", "", 3.5, Pad},
  {"", "", 0.0, EndRow},
  {"Abc", "Abc", 1.0, BackLayer, .scheme = 1},
  {"Ctr", "Ctr", 1.0, Mod, Ctrl, .scheme = 1},
  {"", "", 8, Pad},
  {"", "", 0.0, Last},
};
