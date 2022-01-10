#include "proto/virtual-keyboard-unstable-v1-client-protocol.h"
#include "proto/wlr-layer-shell-unstable-v1-client-protocol.h"
#include <linux/input-event-codes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>
#include <wchar.h>

#include "keyboard.h"
#include "config.h"

/* lazy die macro */
#define die(...)                                                               \
	fprintf(stderr, __VA_ARGS__);                                                \
	exit(1)

/* client state */
static const char *namespace = "wlroots";
static struct wl_display *display;
static struct wl_compositor *compositor;
static struct wl_seat *seat;
static struct wl_pointer *pointer;
static struct wl_touch *touch;
static struct wl_output *wl_output;
static struct zwlr_layer_shell_v1 *layer_shell;
static struct zwlr_layer_surface_v1 *layer_surface;
static struct zwp_virtual_keyboard_manager_v1 *vkbd_mgr;

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
static bool cur_press = false;
static struct kbd keyboard;
static uint32_t height, normal_height, landscape_height;

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

char *
estrdup(const char *s) {
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
	kbd_release_key(&keyboard, time);
}

void
wl_touch_motion(void *data, struct wl_touch *wl_touch, uint32_t time,
                int32_t id, wl_fixed_t x, wl_fixed_t y) {
	uint32_t touch_x, touch_y;

	touch_x = wl_fixed_to_int(x);
	touch_y = wl_fixed_to_int(y);

	kbd_motion_key(&keyboard, time, touch_x, touch_y);
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

	if (cur_press) {
		kbd_motion_key(&keyboard, time, cur_x, cur_y);
	}
}

void
wl_pointer_button(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
                  uint32_t time, uint32_t button, uint32_t state) {
	struct key *next_key;
	cur_press = state == WL_POINTER_BUTTON_STATE_PRESSED;

	if (cur_press) {
		kbd_unpress_key(&keyboard, time);
	} else {
		kbd_release_key(&keyboard, time);
	}

	if (cur_press && cur_x >= 0 && cur_y >= 0) {
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

static void
display_handle_geometry(void *data, struct wl_output *wl_output, int x, int y,
                        int physical_width, int physical_height, int subpixel,
                        const char *make, const char *model, int transform) {
	if (transform % 2 == 0 && keyboard.landscape) {
		keyboard.landscape = false;
		height = normal_height;
	} else if (transform % 2 != 0 && !keyboard.landscape) {
		keyboard.landscape = true;
		height = landscape_height;
	} else {
		return; // no changes
	}

	enum layout_id layer;
	if (keyboard.landscape) {
		layer = keyboard.landscape_layers[0];
	} else {
		layer = keyboard.layers[0];
	}

	keyboard.layout = &keyboard.layouts[layer];
	keyboard.prevlayout = keyboard.layout;

	zwlr_layer_surface_v1_set_size(layer_surface, 0, height);
	zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, height);
	wl_surface_commit(draw_surf.surf);
}

static void
display_handle_done(void *data, struct wl_output *wl_output) {}

static void
display_handle_scale(void *data, struct wl_output *wl_output, int32_t scale) {
	keyboard.s = scale;
}

static void
display_handle_mode(void *data, struct wl_output *wl_output, uint32_t flags,
                    int width, int height, int refresh) {}

static const struct wl_output_listener output_listener = {
  .geometry = display_handle_geometry,
  .mode = display_handle_mode,
  .done = display_handle_done,
  .scale = display_handle_scale};

void
handle_global(void *data, struct wl_registry *registry, uint32_t name,
              const char *interface, uint32_t version) {
	if (strcmp(interface, wl_compositor_interface.name) == 0) {
		compositor = wl_registry_bind(registry, name, &wl_compositor_interface, 3);
	} else if (strcmp(interface, wl_shm_interface.name) == 0) {
		draw_ctx.shm = wl_registry_bind(registry, name, &wl_shm_interface, 1);
	} else if (strcmp(interface, "wl_output") == 0) {
		if (!wl_output) {
			wl_output = wl_registry_bind(registry, name, &wl_output_interface, 2);
			keyboard.s = 1;
			wl_output_add_listener(wl_output, &output_listener, NULL);
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
	keyboard.w = w + KBD_PIXEL_OVERSCAN_WIDTH;
	keyboard.h = h;
	kbd_resize(&keyboard, layouts, NumLayouts);

	zwlr_layer_surface_v1_ack_configure(surface, serial);
}

void
layer_surface_closed(void *data, struct zwlr_layer_surface_v1 *surface) {
	zwlr_layer_surface_v1_destroy(surface);
	wl_surface_destroy(draw_surf.surf);
	run_display = false;
}

void
usage(char *argv0) {
	fprintf(stderr,
	        "usage: %s [-hov] [-H height] [-L landscape height] [-fn font] [-l "
	        "layers]\n",
	        argv0);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  -D         - Enable debug\n");
	fprintf(stderr, "  -o         - Print pressed keys to standard output\n");
	fprintf(stderr, "  -O         - Print intersected keys to standard output\n");
	fprintf(stderr, "  -l         - Comma separated list of layers\n");
	fprintf(stderr, "  -H [int]   - Height in pixels\n");
	fprintf(stderr, "  -L [int]   - Landscape height in pixels\n");
	fprintf(stderr, "  --fn [font] - Set font (e.g: DejaVu Sans 20)\n");
	fprintf(stderr, "  --hidden   - Start hidden (send SIGUSR2 to show)\n");
}

void
hide(int sigint) {
	signal(SIGUSR1, hide);
	if (!layer_surface) {
		return;
	}

	zwlr_layer_surface_v1_destroy(layer_surface);
	wl_surface_destroy(draw_surf.surf);
	layer_surface = NULL;
	if (draw_surf.cb) {
		wl_callback_destroy(draw_surf.cb);
		draw_surf.cb = NULL;
	}
}

void
show(int sigint) {
	signal(SIGUSR2, show);
	if (layer_surface) {
		return;
	}

	wl_display_sync(display);

	draw_surf.surf = wl_compositor_create_surface(compositor);
	;
	layer_surface = zwlr_layer_shell_v1_get_layer_surface(
	  layer_shell, draw_surf.surf, wl_output, layer, namespace);

	zwlr_layer_surface_v1_set_size(layer_surface, 0, height);
	zwlr_layer_surface_v1_set_anchor(layer_surface, anchor);
	zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, height);
	zwlr_layer_surface_v1_set_keyboard_interactivity(layer_surface, false);
	zwlr_layer_surface_v1_add_listener(layer_surface, &layer_surface_listener,
	                                   NULL);
	wl_surface_commit(draw_surf.surf);

	wl_display_roundtrip(display);
	drwsurf_flip(&draw_surf);
}

int
main(int argc, char **argv) {
	/* parse command line arguments */
	char *layer_names_list = NULL;
	const char *fc_font_pattern = NULL;
	height = normal_height = KBD_PIXEL_HEIGHT;
	landscape_height = KBD_PIXEL_LANDSCAPE_HEIGHT;

	char *tmp;
	if ((tmp = getenv("WVKBD_LAYERS")))
		layer_names_list = estrdup(tmp);
	if ((tmp = getenv("WVKBD_HEIGHT")))
		normal_height = atoi(tmp);
	if ((tmp = getenv("WVKBD_LANDSCAPE_HEIGHT")))
		landscape_height = atoi(tmp);

	/* keyboard settings */
	keyboard.layers = (enum layout_id *)&layers;
	keyboard.landscape_layers = (enum layout_id *)&landscape_layers;
	keyboard.scheme = scheme;
	keyboard.layer_index = 0;
	keyboard.scheme1 = scheme1;
	keyboard.scheme1 = scheme1;

	bool starthidden = false;

	int i;
	for (i = 1; argv[i]; i++) {
		if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version"))) {
			printf("wvkbd-%s", VERSION);
			exit(0);
		} else if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help"))) {
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
			height = normal_height = atoi(argv[++i]);
		} else if (!strcmp(argv[i], "-L")) {
			if (i >= argc - 1) {
				usage(argv[0]);
				exit(1);
			}
			landscape_height = atoi(argv[++i]);
		} else if (!strcmp(argv[i], "-D")) {
			keyboard.debug = true;
		} else if ((!strcmp(argv[i], "-fn")) || (!strcmp(argv[i], "--fn"))) {
			fc_font_pattern = estrdup(argv[++i]);
		} else if (!strcmp(argv[i], "-o")) {
			keyboard.print = true;
		} else if (!strcmp(argv[i], "-O")) {
			keyboard.print_intersect = true;
		} else if ((!strcmp(argv[i], "-hidden")) ||
		           (!strcmp(argv[i], "--hidden"))) {
			starthidden = true;
		} else {
			fprintf(stderr, "Invalid argument: %s\n", argv[i]);
			usage(argv[0]);
			exit(1);
		}
	}

	if (!fc_font_pattern) {
		fc_font_pattern = default_font;
	}

	display = wl_display_connect(NULL);
	if (display == NULL) {
		die("Failed to create display\n");
	}

	draw_surf.ctx = &draw_ctx;
	keyboard.surf = &draw_surf;

	struct wl_registry *registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &registry_listener, NULL);
	wl_display_roundtrip(display);

	if (compositor == NULL) {
		die("wl_compositor not available\n");
	}
	if (draw_ctx.shm == NULL) {
		die("wl_shm not available\n");
	}
	if (layer_shell == NULL) {
		die("layer_shell not available\n");
	}
	if (vkbd_mgr == NULL) {
		die("virtual_keyboard_manager not available\n");
	}

	keyboard.vkbd =
	  zwp_virtual_keyboard_manager_v1_create_virtual_keyboard(vkbd_mgr, seat);
	if (keyboard.vkbd == NULL) {
		die("failed to init virtual keyboard_manager\n");
	}

	kbd_init(&keyboard, (struct layout *)&layouts, layer_names_list);

	draw_ctx.font_description =
	  pango_font_description_from_string(fc_font_pattern);

	if (!starthidden) {
		draw_surf.surf = wl_compositor_create_surface(compositor);

		layer_surface = zwlr_layer_shell_v1_get_layer_surface(
		  layer_shell, draw_surf.surf, wl_output, layer, namespace);

		zwlr_layer_surface_v1_set_size(layer_surface, 0, height);
		zwlr_layer_surface_v1_set_anchor(layer_surface, anchor);
		zwlr_layer_surface_v1_set_exclusive_zone(layer_surface, height);
		zwlr_layer_surface_v1_set_keyboard_interactivity(layer_surface, false);
		zwlr_layer_surface_v1_add_listener(layer_surface, &layer_surface_listener,
		                                   NULL);
		wl_surface_commit(draw_surf.surf);

		wl_display_roundtrip(display);
		drwsurf_flip(&draw_surf);
	}

	signal(SIGUSR1, hide);
	signal(SIGUSR2, show);

	while (run_display) {
		while (wl_display_dispatch(display) != -1 && layer_surface) {
		}
		wl_display_roundtrip(display);
		while (run_display && !layer_surface) {
			sleep(1);
		}
	}

	if (fc_font_pattern != default_font) {
		free((void *)fc_font_pattern);
	}

	return 0;
}
