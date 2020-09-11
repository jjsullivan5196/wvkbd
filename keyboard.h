/* clang-format off */
#define KBD_POINTS KBD_ROWS * KBD_COLS
/* clang-format on */

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
	Last,
};

/* Modifiers passed to the virtual_keyboard protocol. They are based on
 * wayland's wl_keyboard, which doesn't document them.
 */
enum key_modifier_type {
	NoMod = 0,
	Shift = 1,
	CapsLock = 2,
	Ctrl = 4,
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
	const char *label;
	const char *shift_label;
	const uint8_t width;
	const enum key_type type;

	const uint32_t code;
	struct layout *layout;

	uint8_t col, row;
};

struct layout {
	struct key *keys;
	struct key *gridpoints[KBD_POINTS];
};

struct kbd {
	struct layout *layout;
	struct clr_scheme scheme;

	uint32_t w, h;
	uint32_t kw, kh;
	uint8_t mods;
	struct key *last_press;

	struct drwsurf *surf;
	struct zwp_virtual_keyboard_v1 *vkbd;
};

static inline void draw_inset(struct drwsurf *d, uint32_t x, uint32_t y,
                              uint32_t width, uint32_t height, uint32_t border,
                              uint32_t color);

static void kbd_init_layout(struct layout *l);
static struct key *kbd_get_key(struct kbd *kb, uint32_t x, uint32_t y);
static void kbd_unpress_key(struct kbd *kb, uint32_t time);
static void kbd_press_key(struct kbd *kb, struct key *k, uint32_t time);
static void kbd_draw_key(struct kbd *kb, struct key *k, bool pressed);
static void kbd_draw_key(struct kbd *kb, struct key *k, bool pressed);
static void kbd_draw_layout(struct kbd *kb);
static void kbd_resize(struct kbd *kb, uint32_t w, uint32_t h);

void
kbd_init_layout(struct layout *l) {
	uint8_t i = 0, width = 0, ncol = 0, col = 0, row = 0;

	struct key *k = l->keys;
	struct key **point = l->gridpoints,
	           **end_point = l->gridpoints + (KBD_POINTS);

	memset(point, 0, sizeof(uint8_t) * KBD_POINTS);

	while ((k->type != Last) && (point < end_point)) {
		width = k->width;
		ncol = col + width;

		if (ncol > KBD_COLS) {
			width = KBD_COLS - col;
			col = 0;
			row += 1;

			for (i = 0; (i < width) && (point < end_point); i++, point++) {
			}
			continue;
		}

		k->col = col;
		k->row = row;
		col = ncol;

		for (i = 0; (i < width) && (point < end_point); i++, point++) {
			*point = k;
		}

		k++;
	}
}

struct key *
kbd_get_key(struct kbd *kb, uint32_t x, uint32_t y) {
	struct layout *l = kb->layout;
	uint32_t col = KBD_COLS * ((float)x / (float)kb->w);
	uint32_t row = KBD_ROWS * ((float)y / (float)kb->h);

	return l->gridpoints[(row * KBD_COLS) + col];
}

void
kbd_unpress_key(struct kbd *kb, uint32_t time) {
	if (kb->last_press) {
		kbd_draw_key(kb, kb->last_press, false);
		kb->surf->dirty = true;

		zwp_virtual_keyboard_v1_key(kb->vkbd, time, kb->last_press->code,
		                            WL_KEYBOARD_KEY_STATE_RELEASED);
		kb->last_press = NULL;
	}
}

void
kbd_press_key(struct kbd *kb, struct key *k, uint32_t time) {
	switch (k->type) {
	case Code:
		kb->last_press = k;
		kbd_draw_key(kb, k, true);
		zwp_virtual_keyboard_v1_key(kb->vkbd, time, kb->last_press->code,
		                            WL_KEYBOARD_KEY_STATE_PRESSED);
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
		kb->layout = k->layout;
		kbd_draw_layout(kb);
	default:
		break;
	}

	kb->surf->dirty = true;
}

void
kbd_draw_key(struct kbd *kb, struct key *k, bool pressed) {
	struct drwsurf *d = kb->surf;
	const char *label = (kb->mods & Shift) ? k->shift_label : k->label;
	Color *fill = pressed ? &kb->scheme.high : &kb->scheme.fg;
	uint32_t x = k->col * kb->kw, y = k->row * kb->kh, width = k->width * kb->kw;

	draw_inset(d, x, y, width, kb->kh, KBD_KEY_BORDER, fill->color);
	wld_draw_text(d->render, d->ctx->font, kb->scheme.text.color,
	              x + (width * 0.35), y + (kb->kh / 2), label, -1, NULL);
}

void
kbd_draw_layout(struct kbd *kb) {
	struct drwsurf *d = kb->surf;
	struct key *next_key = kb->layout->keys;
	bool pressed = false;

	wld_fill_rectangle(d->render, kb->scheme.bg.color, 0, 0, kb->w, kb->h);

	while (next_key->type != Last) {
		if (next_key->type == Pad) {
			next_key++;
			continue;
		}
		pressed = next_key->type == Mod && kb->mods & next_key->code;
		kbd_draw_key(kb, next_key, pressed);
		next_key++;
	}
}

void
kbd_resize(struct kbd *kb, uint32_t w, uint32_t h) {
	struct drwsurf *d = kb->surf;

	kb->w = w;
	kb->h = h;

	kb->kw = (float)w / (float)KBD_COLS;
	kb->kh = (float)h / (float)KBD_ROWS;

	drwsurf_resize(d, w, h);
	kbd_draw_layout(kb);
	d->dirty = true;
}

void
draw_inset(struct drwsurf *d, uint32_t x, uint32_t y, uint32_t width,
           uint32_t height, uint32_t border, uint32_t color) {
	wld_fill_rectangle(d->render, color, x + border, y + border, width - border,
	                   height - border);
}
