#include "proto/virtual-keyboard-unstable-v1-client-protocol.h"
#include "proto/wlr-layer-shell-unstable-v1-client-protocol.h"
#include "wld/wayland.h"
#include "wld/wld.h"
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <wayland-client.h>
#include <wchar.h>

#include "drw.h"
#include "os-compatibility.h"

/* lazy die macro */
#define die(...)                                                               \
	fprintf(stderr, __VA_ARGS__);                                                \
	exit(1)

/* client state */
static const char *namespace = "wlroots";
static struct wl_display *display;
static struct wl_compositor *compositor;
static struct wl_seat *seat;
static struct wl_shm *shm;
static struct wl_pointer *pointer;
static struct wl_touch *touch;
static struct wl_output *wl_output;
static struct wl_surface *wl_surface;
static struct zwlr_layer_shell_v1 *layer_shell;
static struct zwlr_layer_surface_v1 *layer_surface;
static struct zwp_virtual_keyboard_manager_v1 *vkbd_mgr;
static uint32_t output = UINT32_MAX;

/* drawing */
static struct drw draw_ctx;
static struct drwsurf draw_surf;

/* layer surface parameters */
static uint32_t layer = ZWLR_LAYER_SHELL_V1_LAYER_TOP;
static uint32_t anchor = ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM |
                         ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT |
                         ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT;

/* application state */
static bool run_display = true;
static int cur_x = -1, cur_y = -1;
static int compose = 0;

/* event handler prototypes */
static void wl_pointer_enter(void *data, struct wl_pointer *wl_pointer,
                             uint32_t serial, struct wl_surface *surface,
                             wl_fixed_t surface_x, wl_fixed_t surface_y);
static void wl_pointer_leave(void *data, struct wl_pointer *wl_pointer,
                             uint32_t serial, struct wl_surface *surface);
static void wl_pointer_motion(void *data, struct wl_pointer *wl_pointer,
                              uint32_t time, wl_fixed_t surface_x,
                              wl_fixed_t surface_y);
static void wl_pointer_button(void *data, struct wl_pointer *wl_pointer,
                              uint32_t serial, uint32_t time, uint32_t button,
                              uint32_t state);

static void wl_touch_down(void *data, struct wl_touch *wl_touch,
                          uint32_t serial, uint32_t time,
                          struct wl_surface *surface, int32_t id, wl_fixed_t x,
                          wl_fixed_t y);
static void wl_touch_up(void *data, struct wl_touch *wl_touch, uint32_t serial,
                        uint32_t time, int32_t id);
static void wl_touch_motion(void *data, struct wl_touch *wl_touch,
                            uint32_t time, int32_t id, wl_fixed_t x,
                            wl_fixed_t y);
static void wl_touch_frame(void *data, struct wl_touch *wl_touch);
static void wl_touch_cancel(void *data, struct wl_touch *wl_touch);
static void wl_touch_shape(void *data, struct wl_touch *wl_touch, int32_t id,
                           wl_fixed_t major, wl_fixed_t minor);
static void wl_touch_orientation(void *data, struct wl_touch *wl_touch,
                                 int32_t id, wl_fixed_t orientation);

static void seat_handle_capabilities(void *data, struct wl_seat *wl_seat,
                                     enum wl_seat_capability caps);
static void seat_handle_name(void *data, struct wl_seat *wl_seat,
                             const char *name);

static void handle_global(void *data, struct wl_registry *registry,
                          uint32_t name, const char *interface,
                          uint32_t version);
static void handle_global_remove(void *data, struct wl_registry *registry,
                                 uint32_t name);

static void layer_surface_configure(void *data,
                                    struct zwlr_layer_surface_v1 *surface,
                                    uint32_t serial, uint32_t w, uint32_t h);
static void layer_surface_closed(void *data,
                                 struct zwlr_layer_surface_v1 *surface);
static void create_and_upload_keymap(const char *name, uint32_t comp_unichr,
                                     uint32_t comp_shift_unichr);

/* event handlers */
static const struct wl_pointer_listener pointer_listener = {
  .enter = wl_pointer_enter,
  .leave = wl_pointer_leave,
  .motion = wl_pointer_motion,
  .button = wl_pointer_button,
};

static const struct wl_touch_listener touch_listener = {
  .down = wl_touch_down,
  .up = wl_touch_up,
  .motion = wl_touch_motion,
  .frame = wl_touch_frame,
  .cancel = wl_touch_cancel,
  .shape = wl_touch_shape,
  .orientation = wl_touch_orientation,
};

static const struct wl_seat_listener seat_listener = {
  .capabilities = seat_handle_capabilities,
  .name = seat_handle_name,
};

static const struct wl_registry_listener registry_listener = {
  .global = handle_global,
  .global_remove = handle_global_remove,
};

static const struct zwlr_layer_surface_v1_listener layer_surface_listener = {
  .configure = layer_surface_configure,
  .closed = layer_surface_closed,
};

/* configuration, allows nested code to access above variables */
#ifndef LAYOUT
#error "make sure to define LAYOUT"
#endif
#include LAYOUT

char *
estrdup(const char *s)
{
	char *p;

	if (!(p = strdup(s))) {
		fprintf(stderr, "strdup:");
		exit(6);
	}

	return p;
}

void
wl_touch_down(void *data, struct wl_touch *wl_touch, uint32_t serial,
              uint32_t time, struct wl_surface *surface, int32_t id,
              wl_fixed_t x, wl_fixed_t y) {
	struct key *next_key;
	uint32_t touch_x, touch_y;

	touch_x = wl_fixed_to_int(x);
	touch_y = wl_fixed_to_int(y);

	kbd_unpress_key(&keyboard, time);

	next_key = kbd_get_key(&keyboard, touch_x, touch_y);
	if (next_key) {
		kbd_press_key(&keyboard, next_key, time);
	}
}

void
wl_touch_up(void *data, struct wl_touch *wl_touch, uint32_t serial,
            uint32_t time, int32_t id) {
	kbd_unpress_key(&keyboard, time);
}

void
wl_touch_motion(void *data, struct wl_touch *wl_touch, uint32_t time,
                int32_t id, wl_fixed_t x, wl_fixed_t y) {
	kbd_unpress_key(&keyboard, time);
}

void
wl_touch_frame(void *data, struct wl_touch *wl_touch) {}

void
wl_touch_cancel(void *data, struct wl_touch *wl_touch) {}

void
wl_touch_shape(void *data, struct wl_touch *wl_touch, int32_t id,
               wl_fixed_t major, wl_fixed_t minor) {}

void
wl_touch_orientation(void *data, struct wl_touch *wl_touch, int32_t id,
                     wl_fixed_t orientation) {}

void
wl_pointer_enter(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
                 struct wl_surface *surface, wl_fixed_t surface_x,
                 wl_fixed_t surface_y) {}

void
wl_pointer_leave(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
                 struct wl_surface *surface) {
	cur_x = cur_y = -1;
}

void
wl_pointer_motion(void *data, struct wl_pointer *wl_pointer, uint32_t time,
                  wl_fixed_t surface_x, wl_fixed_t surface_y) {
	cur_x = wl_fixed_to_int(surface_x);
	cur_y = wl_fixed_to_int(surface_y);

	kbd_unpress_key(&keyboard, time);
}

void
wl_pointer_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
                  uint32_t time, uint32_t button, uint32_t state) {
	struct key *next_key;
	bool press = state == WL_POINTER_BUTTON_STATE_PRESSED;

	kbd_unpress_key(&keyboard, time);

	if (press && cur_x >= 0 && cur_y >= 0) {
		next_key = kbd_get_key(&keyboard, cur_x, cur_y);
		if (next_key) {
			kbd_press_key(&keyboard, next_key, time);
		}
	}
}

void
seat_handle_capabilities(void *data, struct wl_seat *wl_seat,
                         enum wl_seat_capability caps) {
	if ((caps & WL_SEAT_CAPABILITY_POINTER)) {
		pointer = wl_seat_get_pointer(wl_seat);
		wl_pointer_add_listener(pointer, &pointer_listener, NULL);
	}
	if ((caps & WL_SEAT_CAPABILITY_TOUCH)) {
		touch = wl_seat_get_touch(wl_seat);
		wl_touch_add_listener(touch, &touch_listener, NULL);
	}
}

void
seat_handle_name(void *data, struct wl_seat *wl_seat, const char *name) {}

void
handle_global(void *data, struct wl_registry *registry, uint32_t name,
              const char *interface, uint32_t version) {
	if (strcmp(interface, wl_compositor_interface.name) == 0) {
		compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 1);
	} else if (strcmp(interface, wl_shm_interface.name) == 0) {
		shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
	} else if (strcmp(interface, "wl_output") == 0) {
		if (output != UINT32_MAX) {
			if (!wl_output) {
				wl_output = wl_registry_bind(registry, name, &wl_output_interface, 1);
			} else {
				output--;
			}
		}
	} else if (strcmp(interface, wl_seat_interface.name) == 0) {
		seat = wl_registry_bind(registry, name, &wl_seat_interface, 1);
		wl_seat_add_listener(seat, &seat_listener, NULL);
	} else if (strcmp(interface, zwlr_layer_shell_v1_interface.name) == 0) {
		layer_shell =
		  wl_registry_bind(registry, name, &zwlr_layer_shell_v1_interface, 1);
	} else if (strcmp(interface,
	                  zwp_virtual_keyboard_manager_v1_interface.name) == 0) {
		vkbd_mgr = wl_registry_bind(registry, name,
		                            &zwp_virtual_keyboard_manager_v1_interface, 1);
	}
}

void
handle_global_remove(void *data, struct wl_registry *registry, uint32_t name) {}

void
layer_surface_configure(void *data, struct zwlr_layer_surface_v1 *surface,
                        uint32_t serial, uint32_t w, uint32_t h) {
	kbd_resize(&keyboard, w + KBD_PIXEL_OVERSCAN_WIDTH, h, layouts, NumLayouts);
	zwlr_layer_surface_v1_ack_configure(surface, serial);
}

void
layer_surface_closed(void *data, struct zwlr_layer_surface_v1 *surface) {
	zwlr_layer_surface_v1_destroy(surface);
	wl_surface_destroy(draw_surf.surf);
	run_display = false;
}

void
create_and_upload_keymap(const char *name, uint32_t comp_unichr,
                         uint32_t comp_shift_unichr) {
	const char *keymap_str = get_keymap(name, comp_unichr, comp_shift_unichr);
	size_t keymap_size = strlen(keymap_str) + 1;
	int keymap_fd = os_create_anonymous_file(keymap_size);
	if (keymap_fd < 0) {
		die("could not create keymap fd\n");
	}
	void *ptr =
	  mmap(NULL, keymap_size, PROT_READ | PROT_WRITE, MAP_SHARED, keymap_fd, 0);
	if (ptr == (void *)-1) {
		die("could not map keymap data\n");
	}
	strcpy(ptr, keymap_str);
	zwp_virtual_keyboard_v1_keymap(
	  keyboard.vkbd, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, keymap_fd, keymap_size);
	free((void *)keymap_str);
}

void
usage(char *argv0)
{
	fprintf(stderr, "usage: %s [-hov] [-H height] [-fn font] [-l layers]\n", argv0);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -D         - Enable debug\n");
	fprintf(stderr, "  -o         - Print press keys to standard output\n");
	fprintf(stderr, "  -l         - Comma separated list of layers\n");
	fprintf(stderr, "  -H [int]   - Height in pixels\n");
	fprintf(stderr, "  -fn [font] - Set font (Xft, e.g: DejaVu Sans:bold:size=20)\n");
}

int
main(int argc, char **argv) {
	/* parse command line arguments */
	int i;
	char *layer_names_list = NULL;
	char *tmp;
	uint32_t height = KBD_PIXEL_HEIGHT;

	if ((tmp = getenv("WVKBD_LAYERS")))
		layer_names_list = estrdup(tmp);
	if ((tmp = getenv("WVKBD_HEIGHT")))
		height = atoi(tmp);


	for (i = 1; argv[i]; i++) {
		if (!strcmp(argv[i], "-v")) {
			printf("wvkbd-%s", VERSION);
			exit(0);
		} else if (!strcmp(argv[i], "-h")) {
			usage(argv[0]);
			exit(0);
		} else if (!strcmp(argv[i], "-l")) {
			if (i >= argc - 1) {
				usage(argv[0]);
				exit(1);
			}
			if (layer_names_list)
				free(layer_names_list);
			layer_names_list = estrdup(argv[++i]);
		} else if (!strcmp(argv[i], "-H")) {
			if (i >= argc - 1) {
				usage(argv[0]);
				exit(1);
			}
			height = atoi(argv[++i]);
		} else if (!strcmp(argv[i], "-o")) {
			keyboard.print = true;
		} else {
			fprintf(stderr, "Invalid argument: %s\n", argv[i]);
			usage(argv[0]);
			exit(1);
		}
	}

	/* connect to compositor */
	display = wl_display_connect(NULL);
	if (display == NULL) {
		die("Failed to create display\n");
	}

	/* acquire state */
	struct wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_listener, NULL);
	wl_display_roundtrip(display);

	if (compositor == NULL) {
		die("wl_compositor not available\n");
	}
	if (shm == NULL) {
		die("wl_shm not available\n");
	}
	if (layer_shell == NULL) {
		die("layer_shell not available\n");
	}
	if (vkbd_mgr == NULL) {
		die("virtual_keyboard_manager not available\n");
	}

	/* create vkbd */
	keyboard.vkbd =
	  zwp_virtual_keyboard_manager_v1_create_virtual_keyboard(vkbd_mgr, seat);

	kbd_init(&keyboard, &layouts, layer_names_list);

	/* assign kbd state */
	keyboard.surf = &draw_surf;

	/* create surface */
	wl_surface = wl_compositor_create_surface(compositor);
	drw_init(&draw_ctx, fc_font_pattern, display, shm);
	drwsurf_init(&draw_ctx, &draw_surf, wl_surface);

	layer_surface = zwlr_layer_shell_v1_get_layer_surface(
	  layer_shell, draw_surf.surf, wl_output, layer, namespace);

	zwlr_layer_surface_v1_set_size(layer_surface, 0, height);
	zwlr_layer_surface_v1_set_anchor(layer_surface, anchor);
	zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, height);
	zwlr_layer_surface_v1_set_keyboard_interactivity(layer_surface, false);
	zwlr_layer_surface_v1_add_listener(layer_surface, &layer_surface_listener,
	                                   NULL);
	wl_surface_commit(draw_surf.surf);

	/* flush requests and start drawing */
	wl_display_roundtrip(display);
	drwsurf_flip(&draw_surf);

	while (wl_display_dispatch(display) != -1 && run_display) {
	}

	return 0;
}
