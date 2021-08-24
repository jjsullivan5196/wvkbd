#define MAX_LAYERS 25

enum key_type;
enum key_modifier_type;
struct clr_scheme;
struct key;
struct layout;
struct kbd;

enum key_type {
	Pad = 0,    //Padding, not a pressable key
	Code,       //A normal key emitting a keycode
	Mod,        //A modifier key
	Copy,       //Copy key, copies the unicode value specified in code (creates and activates temporary keymap)
	            // used for keys that are not part of the keymap
	Layout,     //Layout switch to a specific layout
	BackLayer,  //Layout switch to the layout that was previously active
	NextLayer,  //Layout switch to the next layout in the layers sequence
	Compose,    //Compose modifier key, switches to a specific associated layout upon next keypress
	EndRow,     //Incidates the end of a key row
	Last,       //Indicated the end of a layout
};

/* Modifiers passed to the virtual_keyboard protocol. They are based on
 * wayland's wl_keyboard, which doesn't document them.
 */
enum key_modifier_type {
	NoMod = 0,
	Shift = 1,
	CapsLock = 2,
	Ctrl = 4,
	Alt = 8,
	Super = 64,
	AltGr = 128,
};

struct clr_scheme {
	Color fg;
	Color bg;
	Color high;
	Color text;
};

struct key {
	const char *label;       // primary label
	const char *shift_label; // secondary label
	const double width;      // relative width (1.0)
	const enum key_type type;

	const uint32_t
	  code;                /* code: key scancode or modifier name (see
	                        *   `/usr/include/linux/input-event-codes.h` for scancode names, and
	                        *   `keyboard.h` for modifiers)
	                        *   XKB keycodes are +8 */
	struct layout *layout; // pointer back to the parent layout that holds this
	                       // key
	const uint32_t code_mod; /* modifier to force when this key is pressed */
	uint8_t scheme;          // index of the scheme to use
	bool reset_mod;          /* reset modifiers when clicked */

	// actual coordinates on the surface (pixels), will be computed automatically
	// for all keys
	uint32_t x, y, w, h;
};

struct layout {
	struct key *keys;
	const char *keymap_name;
	const char *name;
	uint32_t keyheight; // absolute height (pixels)
};

struct kbd {
	struct layout *layout;
	struct clr_scheme scheme;
	struct clr_scheme scheme1;

	bool print;
	uint32_t w, h;
	uint8_t mods;
	struct key *last_press;
	struct layout *prevlayout;
	size_t layer_index;

	struct layout *layouts;
	enum layout_id *layers;

	struct drwsurf *surf;
	struct zwp_virtual_keyboard_v1 *vkbd;
};

static inline void draw_inset(struct drwsurf *d, uint32_t x, uint32_t y,
                              uint32_t width, uint32_t height, uint32_t border,
                              Color color);

static void kbd_init(struct kbd *kb, struct layout * layouts, char * layer_names_list);
static void kbd_init_layout(struct layout *l, uint32_t width, uint32_t height);
static struct key *kbd_get_key(struct kbd *kb, uint32_t x, uint32_t y);
static void kbd_unpress_key(struct kbd *kb, uint32_t time);
static void kbd_press_key(struct kbd *kb, struct key *k, uint32_t time);
static void kbd_print_key_stdout(struct kbd *kb, struct key *k);
static void kbd_draw_key(struct kbd *kb, struct key *k, bool pressed);
static void kbd_draw_layout(struct kbd *kb);
static void kbd_resize(struct kbd *kb, uint32_t w, uint32_t h,
                       struct layout *layouts, uint8_t layoutcount);
static uint8_t kbd_get_rows(struct layout *l);
static double kbd_get_row_length(struct key *k);
static void kbd_switch_layout(struct kbd *kb, struct layout *l);

void
kbd_switch_layout(struct kbd *kb, struct layout *l) {
	kb->prevlayout = kb->layout;
	kb->layout = l;
	if (debug) fprintf(stderr, "Switching to layout %s)\n", kb->layout->name);
	if ((!kb->prevlayout) ||
		(strcmp(kb->prevlayout->keymap_name, kb->layout->keymap_name) != 0)) {
		fprintf(stderr, "Switching to keymap %s\n", kb->layout->keymap_name);
		create_and_upload_keymap(kb->layout->keymap_name, 0, 0);
	}
	kbd_draw_layout(kb);
}

uint8_t
kbd_get_rows(struct layout *l) {
	uint8_t rows = 0;
	struct key *k = l->keys;
	while (k->type != Last) {
		if (k->type == EndRow) {
			rows++;
		}
		k++;
	}
	return rows + 1;
}


void kbd_init(struct kbd *kb, struct layout * layouts, char * layer_names_list) {
	char *s;
	int i;
	bool found;

	fprintf(stderr, "Initializing keyboard\n");

	kb->layouts = layouts;

	for (i = 0; i < NumLayouts - 1; i++);
	fprintf(stderr, "Found %d layouts\n",i);

	kb->layer_index = 0;

	if (layer_names_list) {
		uint8_t numlayers = 0;
		kb->layers = malloc(MAX_LAYERS * sizeof(enum layout_id));
		s = strtok(layer_names_list, ",");
		while (s != NULL) {
			if (numlayers + 1 == MAX_LAYERS) {
				fprintf(stderr, "too many layers specified");
				exit(3);
			}
			found = false;
			for (i = 0; i < NumLayouts - 1; i++) {
				if (kb->layouts[i].name && strcmp(kb->layouts[i].name, s) == 0) {
					fprintf(stderr, "layer #%d = %s\n", numlayers + 1, s);
					kb->layers[numlayers++] = i;
					found = true;
					break;
				}
			}
			if (!found) {
				fprintf(stderr, "No such layer: %s\n", s);
				exit(3);
			}
			s = strtok(NULL,",");
		}
		kb->layers[numlayers] = NumLayouts; //mark the end of the sequence
		if (numlayers == 0) {
			fprintf(stderr, "No layers defined\n");
			exit(3);
		}
	}

	i = 0;
	enum layout_id lid = kb->layers[0];
	while (lid != NumLayouts) {
		lid = kb->layers[++i];
	}
	fprintf(stderr, "Found %d layers\n",i);

	kb->layout = &kb->layouts[kb->layers[kb->layer_index]];
	kb->prevlayout = kb->layout;

	/* upload keymap */
	create_and_upload_keymap(kb->layout->keymap_name, 0, 0);
}

void
kbd_init_layout(struct layout *l, uint32_t width, uint32_t height) {
	uint32_t x = 0, y = 0;
	if (debug) fprintf(stderr, "Init layout %s\n", l->name);
	uint8_t rows = kbd_get_rows(l);

	l->keyheight = height / rows;

	struct key *k = l->keys;
	double rowlength = kbd_get_row_length(k);
	while (k->type != Last) {
		if (k->type == EndRow) {
			y += l->keyheight;
			x = 0;
			rowlength = kbd_get_row_length(k + 1);
		} else if (k->width > 0) {
			k->x = x;
			k->y = y;
			k->w = ((double)width / rowlength) * k->width;
			x += k->w;
		}
		k->h = l->keyheight;
		k++;
	}
}

double
kbd_get_row_length(struct key *k) {
	double l = 0.0;
	while ((k->type != Last) && (k->type != EndRow)) {
		l += k->width;
		k++;
	}
	return l;
}

struct key *
kbd_get_key(struct kbd *kb, uint32_t x, uint32_t y) {
	struct layout *l = kb->layout;
	struct key *k = l->keys;
	if (debug) fprintf(stderr, "get key: +%d+%d\n", x, y);
	while (k->type != Last) {
		if ((k->type != EndRow) && (k->type != Pad) && (k->type != Pad) &&
		    (x >= k->x) && (y >= k->y) && (x < k->x + k->w) && (y < k->y + k->h)) {
			return k;
		}
		k++;
	}
	return NULL;
}

void
kbd_unpress_key(struct kbd *kb, uint32_t time) {
	if (kb->last_press) {
		kbd_draw_key(kb, kb->last_press, false);
		kb->surf->dirty = true;

		if (kb->last_press->type == Copy) {
			zwp_virtual_keyboard_v1_key(kb->vkbd, time, 127, // COMP key
			                            WL_KEYBOARD_KEY_STATE_RELEASED);
		} else {
			zwp_virtual_keyboard_v1_key(kb->vkbd, time, kb->last_press->code,
			                            WL_KEYBOARD_KEY_STATE_RELEASED);
		}
		kb->last_press = NULL;

		if (compose >= 2) {
			compose = 0;
			kbd_switch_layout(kb, kb->prevlayout);
			if ((kb->mods & Shift) == Shift)
				kb->mods ^= Shift;
		} else if ((kb->mods & Shift) == Shift) {
			kb->mods ^= Shift;
			kbd_draw_layout(kb);
		}
	}
}

void
kbd_press_key(struct kbd *kb, struct key *k, uint32_t time) {
	if ((compose == 1) && (k->type != Compose) && (k->type != Mod) &&
	    (k->layout)) {
		compose++;
		if (debug) fprintf(stderr, "showing compose %d\n", compose);
		kbd_switch_layout(kb, k->layout);
		kb->surf->dirty = true;
		return;
	}

	switch (k->type) {
	case Code:
		if (k->code_mod) {
			if (k->reset_mod) {
				zwp_virtual_keyboard_v1_modifiers(kb->vkbd, k->code_mod, 0, 0, 0);
			} else {
				zwp_virtual_keyboard_v1_modifiers(kb->vkbd, kb->mods ^ k->code_mod, 0,
				                                  0, 0);
			}
		} else {
			zwp_virtual_keyboard_v1_modifiers(kb->vkbd, kb->mods, 0, 0, 0);
		}
		kb->last_press = k;
		kbd_draw_key(kb, k, true);
		zwp_virtual_keyboard_v1_key(kb->vkbd, time, kb->last_press->code,
		                            WL_KEYBOARD_KEY_STATE_PRESSED);
		if (kb->print)
			kbd_print_key_stdout(kb, k);
		if (compose) {
			if (debug) fprintf(stderr, "pressing composed key\n");
			compose++;
		}
		break;
	case Mod:
		kb->mods ^= k->code;
		if (k->code == Shift) {
			kbd_draw_layout(kb);
		}
		kbd_draw_key(kb, k, kb->mods & k->code);
		zwp_virtual_keyboard_v1_modifiers(kb->vkbd, kb->mods, 0, 0, 0);
		break;
	case Layout:
		//switch to the layout determined by the key
		kbd_switch_layout(kb, k->layout);
		break;
	case Compose:
		//switch to the associated layout determined by the *next* keypress
		if (compose == 0) {
			compose = 1;
		} else {
			compose = 0;
		}
		kbd_draw_key(kb, k, (bool)compose);
		break;
	case NextLayer:
		//switch to the next layout in the layer sequence
		kb->layer_index++;
		if (kb->layers[kb->layer_index] == NumLayouts) {
			kb->layer_index = 0;
		}
		kbd_switch_layout(kb, &kb->layouts[kb->layers[kb->layer_index]]);
		break;
	case BackLayer:
		//switch to the previously active layout
		if (kb->prevlayout)
			kbd_switch_layout(kb, kb->prevlayout);
		break;
	case Copy:
		//copy code as unicode chr by setting a temporary keymap
		kb->last_press = k;
		kbd_draw_key(kb, k, true);
		if (debug) fprintf(stderr, "pressing copy key\n");
		create_and_upload_keymap(kb->layout->keymap_name, k->code, k->code_mod);
		zwp_virtual_keyboard_v1_modifiers(kb->vkbd, kb->mods, 0, 0, 0);
		zwp_virtual_keyboard_v1_key(kb->vkbd, time, 127, // COMP key
		                            WL_KEYBOARD_KEY_STATE_PRESSED);
		if (kb->print)
			kbd_print_key_stdout(kb, k);
		break;
	default:
		break;
	}

	kb->surf->dirty = true;
}


void
kbd_print_key_stdout(struct kbd *kb, struct key *k) {
	/* printed keys may slightly differ from the actual output
	 * we generally print what is on the key LABEL and only support the normal
	 * and shift layers. Other modifiers produce no output (Ctrl,Alt)
	 * */

	bool handled = true;
	if (k->type == Code) {
		switch (k->code) {
			case KEY_SPACE:
				printf(" ");
				break;
			case KEY_ENTER:
				printf("\n");
				break;
			case KEY_BACKSPACE:
				printf("\b");
				break;
			case KEY_TAB:
				printf("\t");
				break;
			default:
				handled = false;
				break;
		}
	} else if (k->type != Copy) {
		return;
	}

	if (!handled) {
		if ((kb->mods & Shift) || (kb->mods & CapsLock))
			printf("%s", k->shift_label);
		else if (!(kb->mods & Ctrl) && !(kb->mods & Alt) && !(kb->mods & Super))
			printf("%s", k->label);
	}
	fflush(stdout);
}

void
kbd_draw_key(struct kbd *kb, struct key *k, bool pressed) {
	struct drwsurf *d = kb->surf;
	const char *label = (kb->mods & Shift) ? k->shift_label : k->label;
	if (debug) fprintf(stderr, "Draw key +%d+%d %dx%d -> %s\n", k->x, k->y, k->w, k->h,
	        label);
	struct clr_scheme *scheme = (k->scheme == 0) ? &(kb->scheme) : &(kb->scheme1);
	Color *fill = pressed ? &scheme->high : &scheme->fg;
	draw_inset(d, k->x, k->y, k->w, k->h, KBD_KEY_BORDER, *fill);
	drw_draw_text(d, scheme->text, k->x, k->y, k->w, k->h, label);
}

void
kbd_draw_layout(struct kbd *kb) {
	struct drwsurf *d = kb->surf;
	struct key *next_key = kb->layout->keys;
	bool pressed = false;
	if (debug) fprintf(stderr, "Draw layout");

	drw_fill_rectangle(d, kb->scheme.bg, 0, 0, kb->w, kb->h);

	while (next_key->type != Last) {
		if ((next_key->type == Pad) || (next_key->type == EndRow)) {
			next_key++;
			continue;
		}
		pressed = next_key->type == Mod && kb->mods & next_key->code;
		kbd_draw_key(kb, next_key, pressed);
		next_key++;
	}
}

void
kbd_resize(struct kbd *kb, uint32_t w, uint32_t h, struct layout *layouts,
           uint8_t layoutcount) {
	struct drwsurf *d = kb->surf;

	kb->w = w;
	kb->h = h;

	fprintf(stderr, "Resize %dx%d, %d layouts\n", w, h, layoutcount);

	drwsurf_resize(d, w, h);
	for (int i = 0; i < layoutcount; i++) {
		kbd_init_layout(&layouts[i], w, h);
	}
	kbd_draw_layout(kb);
	d->dirty = true;
}

void
draw_inset(struct drwsurf *d, uint32_t x, uint32_t y, uint32_t width,
           uint32_t height, uint32_t border, Color color) {
	drw_fill_rectangle(d, color, x + border, y + border, width - border,
	                   height - border);
}
