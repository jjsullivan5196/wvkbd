enum key_type;
enum key_modifier_type;
struct clr_scheme;
struct key;
struct layout;
struct kbd;

enum key_type {
	Pad = 0,
	Code,
	Mod,
	Layout,
	EndRow,
	Last,
	Compose,
	Copy,
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
	const char *label;              //primary label
	const char *shift_label;        //secondary label
	const double width;             //relative width (1.0)
	const enum key_type type;

	const uint32_t code;  /* code: key scancode or modifier name (see
						   *   `/usr/include/linux/input-event-codes.h` for scancode names, and
						   *   `keyboard.h` for modifiers)
						   *   XKB keycodes are +8 */
	struct layout *layout; //pointer back to the parent layout that holds this key
	const uint32_t code_mod; /* modifier to force when this key is pressed */

	//actual coordinates on the surface (pixels), will be computed automatically for all keys
	uint32_t x, y, w, h;
};

struct layout {
	struct key *keys;
	uint32_t keyheight; //absolute height (pixels)
};

struct kbd {
	struct layout *layout;
	struct layout *prevlayout;
	struct clr_scheme scheme;

	uint32_t w, h;
	uint8_t mods;
	struct key *last_press;

	struct drwsurf *surf;
	struct zwp_virtual_keyboard_v1 *vkbd;
};

static inline void draw_inset(struct drwsurf *d, uint32_t x, uint32_t y,
                              uint32_t width, uint32_t height, uint32_t border,
                              uint32_t color);

static void kbd_init_layout(struct layout *l, uint32_t width, uint32_t height);
static struct key *kbd_get_key(struct kbd *kb, uint32_t x, uint32_t y);
static void kbd_unpress_key(struct kbd *kb, uint32_t time);
static void kbd_press_key(struct kbd *kb, struct key *k, uint32_t time);
static void kbd_draw_key(struct kbd *kb, struct key *k, bool pressed);
static void kbd_draw_layout(struct kbd *kb);
static void kbd_resize(struct kbd *kb, uint32_t w, uint32_t h, struct layout * layouts, uint8_t layoutcount);
static uint8_t kbd_get_rows(struct layout *l);
static double kbd_get_row_length(struct key *k);

uint8_t kbd_get_rows(struct layout *l) {
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

void
kbd_init_layout(struct layout *l, uint32_t width, uint32_t height) {
	uint32_t x = 0, y = 0;
    fprintf(stderr, "Init layout\n");
	uint8_t rows = kbd_get_rows(l);

	l->keyheight = height / rows;

	struct key *k = l->keys;
	double rowlength = kbd_get_row_length(k);
	while (k->type != Last) {
		if (k->type == EndRow) {
			y += l->keyheight;
			x = 0;
			rowlength = kbd_get_row_length(k+1);
		} else if (k->width > 0) {
			k->x = x;
			k->y = y;
			fprintf(stderr, "(%d/%f)*%f -> %s\n",width,rowlength,k->width, k->label);
			k->w = ((double) width / rowlength) * k->width;
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
	fprintf(stderr,"get key: +%d+%d\n",x,y);
	while (k->type != Last) {
		if ((k->type != EndRow) && (k->type != Pad) && (k->type != Pad) && (x >= k->x) && (y >= k->y) && (x < k->x + k->w) && (y < k->y + k->h)) {
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
			zwp_virtual_keyboard_v1_key(kb->vkbd, time, 127, //COMP key
										WL_KEYBOARD_KEY_STATE_RELEASED);
		} else {
			zwp_virtual_keyboard_v1_key(kb->vkbd, time, kb->last_press->code,
										WL_KEYBOARD_KEY_STATE_RELEASED);
		}
		kb->last_press = NULL;
	}
}

void
kbd_press_key(struct kbd *kb, struct key *k, uint32_t time) {
	switch (k->type) {
	case Code:
		if (k->code_mod) {
			zwp_virtual_keyboard_v1_modifiers(kb->vkbd, k->code_mod, 0, 0, 0);
		} else {
			zwp_virtual_keyboard_v1_modifiers(kb->vkbd, kb->mods, 0, 0, 0);
		}
		if (compose == 1) {
			if (k->layout) {
				compose++;
				if (compose) {
					fprintf(stderr,"showing compose %d\n", compose);
				}
				kb->prevlayout = kb->layout;
				kb->layout = k->layout;
				kbd_draw_layout(kb);
			}
		} else {
			kb->last_press = k;
			kbd_draw_key(kb, k, true);
			zwp_virtual_keyboard_v1_key(kb->vkbd, time, kb->last_press->code,
										WL_KEYBOARD_KEY_STATE_PRESSED);
			if (compose) {
				fprintf(stderr,"pressing composed key\n");
				compose++;
			}
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
	case Compose:
		if (compose == 0) {
			compose = 1;
		} else {
			compose = 0;
		}
		kbd_draw_key(kb, k, (bool) compose);
		break;
	case Layout:
		kb->layout = k->layout;
		kbd_draw_layout(kb);
	case Copy:
		kb->last_press = k;
		kbd_draw_key(kb, k, true);
		fprintf(stderr,"pressing copy key\n");
		create_and_upload_keymap(k->code, k->code_mod);
		zwp_virtual_keyboard_v1_modifiers(kb->vkbd, kb->mods, 0, 0, 0);
		zwp_virtual_keyboard_v1_key(kb->vkbd, time, 127, //COMP key
									WL_KEYBOARD_KEY_STATE_PRESSED);
		break;
	default:
		break;
	}

	if (compose == 3) {
		compose = 0;
		kb->layout = kb->prevlayout;
		kbd_draw_layout(kb);
	}

	kb->surf->dirty = true;
}

void
kbd_draw_key(struct kbd *kb, struct key *k, bool pressed) {
	struct drwsurf *d = kb->surf;
	const char *label = (kb->mods & Shift) ? k->shift_label : k->label;
    fprintf(stderr, "Draw key +%d+%d %dx%d -> %s\n", k->x, k->y, k->w, k->h, k->label);
	Color *fill = pressed ? &kb->scheme.high : &kb->scheme.fg;
	draw_inset(d, k->x, k->y, k->w, k->h, KBD_KEY_BORDER, fill->color);
	uint32_t xoffset = k->w /  (strlen(label) + 2);
    fprintf(stderr, "  xoffset=%d\n", xoffset);
	wld_draw_text(d->render, d->ctx->font, kb->scheme.text.color,
	              k->x + xoffset, k->y + (k->h / 2), label, -1, NULL);
}

void
kbd_draw_layout(struct kbd *kb) {
	struct drwsurf *d = kb->surf;
	struct key *next_key = kb->layout->keys;
	bool pressed = false;
    fprintf(stderr, "Draw layout");

	wld_fill_rectangle(d->render, kb->scheme.bg.color, 0, 0, kb->w, kb->h);

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
kbd_resize(struct kbd *kb, uint32_t w, uint32_t h, struct layout * layouts, uint8_t layoutcount) {
	struct drwsurf *d = kb->surf;

	kb->w = w;
	kb->h = h;

	fprintf(stderr, "Resize %dx%d, %d layouts\n",w,h,layoutcount);

	drwsurf_resize(d, w, h);
	for (int i = 0; i < layoutcount; i++) {
		fprintf(stderr,"i=%d\n",i );
		kbd_init_layout(&layouts[i], w, h);
	}
	kbd_draw_layout(kb);
	d->dirty = true;
}

void
draw_inset(struct drwsurf *d, uint32_t x, uint32_t y, uint32_t width,
           uint32_t height, uint32_t border, uint32_t color) {
	wld_fill_rectangle(d->render, color, x + border, y + border, width - border,
	                   height - border);
}


