#include "proto/virtual-keyboard-unstable-v1-client-protocol.h"
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <sys/mman.h>
#include "keyboard.h"
#include "drw.h"
#include "os-compatibility.h"

#define MAX_LAYERS 25

/* lazy die macro */
#define die(...)                                                               \
	fprintf(stderr, __VA_ARGS__);                                                \
	exit(1)

#ifndef KEYMAP
#error "make sure to define KEYMAP"
#endif
#include KEYMAP

void
kbd_switch_layout(struct kbd *kb, struct layout *l) {
	kb->prevlayout = kb->layout;
	kb->layout = l;
	if (kb->debug)
		fprintf(stderr, "Switching to layout %s)\n", kb->layout->name);
	if ((!kb->prevlayout) ||
	    (strcmp(kb->prevlayout->keymap_name, kb->layout->keymap_name) != 0)) {
		fprintf(stderr, "Switching to keymap %s\n", kb->layout->keymap_name);
		create_and_upload_keymap(kb, kb->layout->keymap_name, 0, 0);
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

void
kbd_init(struct kbd *kb, struct layout *layouts, char *layer_names_list) {
	char *s;
	int i;
	bool found;

	fprintf(stderr, "Initializing keyboard\n");

	kb->layouts = layouts;

	for (i = 0; i < NumLayouts - 1; i++)
		;
	fprintf(stderr, "Found %d layouts\n", i);

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
			s = strtok(NULL, ",");
		}
		kb->layers[numlayers] = NumLayouts; // mark the end of the sequence
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
	fprintf(stderr, "Found %d layers\n", i);

	enum layout_id layer;
	if (kb->landscape) {
		layer = kb->landscape_layers[kb->layer_index];
	} else {
		layer = kb->layers[kb->layer_index];
	}

	kb->layout = &kb->layouts[layer];
	kb->prevlayout = kb->layout;

	/* upload keymap */
	create_and_upload_keymap(kb, kb->layout->keymap_name, 0, 0);
}

void
kbd_init_layout(struct layout *l, uint32_t width, uint32_t height) {
	uint32_t x = 0, y = 0;
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
	if (kb->debug)
		fprintf(stderr, "get key: +%d+%d\n", x, y);
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
	bool unlatch_shift = false;

	if (kb->last_press) {
		unlatch_shift = (kb->mods & Shift) == Shift;

		if (unlatch_shift) {
			kb->mods ^= Shift;
			zwp_virtual_keyboard_v1_modifiers(kb->vkbd, kb->mods, 0, 0, 0);
		}

		if (kb->last_press->type == Copy) {
			zwp_virtual_keyboard_v1_key(kb->vkbd, time, 127, // COMP key
			                            WL_KEYBOARD_KEY_STATE_RELEASED);
		} else {
			zwp_virtual_keyboard_v1_key(kb->vkbd, time, kb->last_press->code,
			                            WL_KEYBOARD_KEY_STATE_RELEASED);
		}

		if (kb->compose >= 2) {
			kb->compose = 0;
			kbd_switch_layout(kb, kb->prevlayout);
		} else if (unlatch_shift) {
			kbd_draw_layout(kb);
		} else {
			kbd_draw_key(kb, kb->last_press, Unpress);
		}

		kb->last_press = NULL;
	}
}

void
kbd_release_key(struct kbd *kb, uint32_t time) {
	kbd_unpress_key(kb, time);
	if (kb->print_intersect && kb->last_swipe) {
		printf("\n");
		// Important so autocompleted words get typed in time
		fflush(stdout);
		kbd_draw_layout(kb);
		kb->last_swipe = NULL;
	}
}

void
kbd_motion_key(struct kbd *kb, uint32_t time, uint32_t x, uint32_t y) {
	// Output intersecting keys
	// (for external 'swiping'-based accelerators).
	if (kb->print_intersect) {
		if (kb->last_press) {
			kbd_unpress_key(kb, time);
			// Redraw last press as a swipe.
			kbd_draw_key(kb, kb->last_swipe, Swipe);
		}
		struct key *intersect_key;
		intersect_key = kbd_get_key(kb, x, y);
		if (intersect_key &&
		    (!kb->last_swipe || intersect_key->label != kb->last_swipe->label)) {
			kbd_print_key_stdout(kb, intersect_key);
			kb->last_swipe = intersect_key;
			kbd_draw_key(kb, kb->last_swipe, Swipe);
		}
	} else {
		kbd_unpress_key(kb, time);
	}
}

void
kbd_press_key(struct kbd *kb, struct key *k, uint32_t time) {
	if ((kb->compose == 1) && (k->type != Compose) && (k->type != Mod) &&
	    (k->layout)) {
		kb->compose++;
		if (kb->debug)
			fprintf(stderr, "showing compose %d\n", kb->compose);
		kbd_switch_layout(kb, k->layout);
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
		kb->last_swipe = kb->last_press = k;
		kbd_draw_key(kb, k, Press);
		zwp_virtual_keyboard_v1_key(kb->vkbd, time, kb->last_press->code,
		                            WL_KEYBOARD_KEY_STATE_PRESSED);
		if (kb->print || kb->print_intersect)
			kbd_print_key_stdout(kb, k);
		if (kb->compose) {
			if (kb->debug)
				fprintf(stderr, "pressing composed key\n");
			kb->compose++;
		}
		break;
	case Mod:
		kb->mods ^= k->code;
		if (k->code == Shift) {
			kbd_draw_layout(kb);
		}
		if (kb->mods & k->code) {
			kbd_draw_key(kb, k, Press);
		} else {
			kbd_draw_key(kb, k, Unpress);
		}
		zwp_virtual_keyboard_v1_modifiers(kb->vkbd, kb->mods, 0, 0, 0);
		break;
	case Layout:
		// switch to the layout determined by the key
		kbd_switch_layout(kb, k->layout);
		break;
	case Compose:
		// switch to the associated layout determined by the *next* keypress
		if (kb->compose == 0) {
			kb->compose = 1;
		} else {
			kb->compose = 0;
		}
		if ((bool)kb->compose) {
			kbd_draw_key(kb, k, Press);
		} else {
			kbd_draw_key(kb, k, Unpress);
		}
		break;
	case NextLayer:
		// switch to the next layout in the layer sequence
		kb->layer_index++;
		enum layout_id layer;
		if (kb->landscape) {
			layer = kb->landscape_layers[kb->layer_index];
		} else {
			layer = kb->layers[kb->layer_index];
		}
		if (layer == NumLayouts) {
			kb->layer_index = 0;
			if (kb->landscape) {
				layer = kb->landscape_layers[kb->layer_index];
			} else {
				layer = kb->layers[kb->layer_index];
			}
		}
		kbd_switch_layout(kb, &kb->layouts[layer]);
		break;
	case BackLayer:
		// switch to the previously active layout
		if (kb->prevlayout)
			kbd_switch_layout(kb, kb->prevlayout);
		break;
	case Copy:
		// copy code as unicode chr by setting a temporary keymap
		kb->last_swipe = kb->last_press = k;
		kbd_draw_key(kb, k, Press);
		if (kb->debug)
			fprintf(stderr, "pressing copy key\n");
		create_and_upload_keymap(kb, kb->layout->keymap_name, k->code, k->code_mod);
		zwp_virtual_keyboard_v1_modifiers(kb->vkbd, kb->mods, 0, 0, 0);
		zwp_virtual_keyboard_v1_key(kb->vkbd, time, 127, // COMP key
		                            WL_KEYBOARD_KEY_STATE_PRESSED);
		if (kb->print || kb->print_intersect)
			kbd_print_key_stdout(kb, k);
		break;
	default:
		break;
	}
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
kbd_draw_key(struct kbd *kb, struct key *k, enum key_draw_type type) {
	struct drwsurf *d = kb->surf;
	const char *label = (kb->mods & Shift) ? k->shift_label : k->label;
	if (kb->debug)
		fprintf(stderr, "Draw key +%d+%d %dx%d -> %s\n", k->x, k->y, k->w, k->h,
		        label);
	struct clr_scheme *scheme = (k->scheme == 0) ? &(kb->scheme) : &(kb->scheme1);
	switch (type) {
	case Unpress:
		draw_inset(d, k->x, k->y, k->w, k->h, KBD_KEY_BORDER, scheme->fg);
		break;
	case Press:
		draw_inset(d, k->x, k->y, k->w, k->h, KBD_KEY_BORDER, scheme->high);
		break;
	case Swipe:
		draw_over_inset(d, k->x, k->y, k->w, k->h, KBD_KEY_BORDER, scheme->swipe);
		break;
	}
	drw_draw_text(d, scheme->text, k->x, k->y, k->w, k->h, label);
}

void
kbd_draw_layout(struct kbd *kb) {
	struct drwsurf *d = kb->surf;
	struct key *next_key = kb->layout->keys;
	if (kb->debug)
		fprintf(stderr, "Draw layout");

	drw_fill_rectangle(d, kb->scheme.bg, 0, 0, kb->w, kb->h);

	while (next_key->type != Last) {
		if ((next_key->type == Pad) || (next_key->type == EndRow)) {
			next_key++;
			continue;
		}
		if (next_key->type == Mod && kb->mods & next_key->code) {
			kbd_draw_key(kb, next_key, Press);
		} else {
			kbd_draw_key(kb, next_key, Unpress);
		}
		next_key++;
	}
}

void
kbd_resize(struct kbd *kb, struct layout *layouts, uint8_t layoutcount) {
	struct drwsurf *d = kb->surf;

	fprintf(stderr, "Resize %dx%d %d, %d layouts\n", kb->w, kb->h, kb->s,
	        layoutcount);

	drwsurf_resize(d, kb->w, kb->h, kb->s);
	for (int i = 0; i < layoutcount; i++) {
		kbd_init_layout(&layouts[i], kb->w, kb->h);
	}
	kbd_draw_layout(kb);
}

void
draw_inset(struct drwsurf *ds, uint32_t x, uint32_t y, uint32_t width,
           uint32_t height, uint32_t border, Color color) {
	drw_fill_rectangle(ds, color, x + border, y + border, width - border,
	                   height - border);
}
void
draw_over_inset(struct drwsurf *ds, uint32_t x, uint32_t y, uint32_t width,
                uint32_t height, uint32_t border, Color color) {
	drw_over_rectangle(ds, color, x + border, y + border, width - border,
	                   height - border);
}

void
create_and_upload_keymap(struct kbd *kb, const char *name, uint32_t comp_unichr,
                         uint32_t comp_shift_unichr) {
	int keymap_index = -1;
	for (int i = 0; i < NUMKEYMAPS; i++) {
		if (!strcmp(keymap_names[i], name)) {
			keymap_index = i;
		}
	}
	if (keymap_index == -1) {
		fprintf(stderr, "No such keymap defined: %s\n", name);
		exit(9);
	}
	const char *keymap_template = keymaps[keymap_index];
	const size_t keymap_size = strlen(keymap_template) + 64;
	char *keymap_str = malloc(keymap_size);
	sprintf(keymap_str, keymap_template, comp_unichr, comp_shift_unichr);
	int keymap_fd = os_create_anonymous_file(keymap_size);
	if (keymap_fd < 0) {
		die("could not create keymap fd\n");
	}
	void *ptr =
	  mmap(NULL, keymap_size, PROT_READ | PROT_WRITE, MAP_SHARED, keymap_fd, 0);
	if (ptr == (void *)-1) {
		die("could not map keymap data\n");
	}
	if (kb->vkbd == NULL) {
		die("kb.vkbd = NULL\n");
	}
	strcpy(ptr, keymap_str);
	zwp_virtual_keyboard_v1_keymap(kb->vkbd, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1,
	                               keymap_fd, keymap_size);
	free((void *)keymap_str);
}
