#ifndef __DRW_H
#define __DRW_H

#include <pango/pangocairo.h>
#include <stdbool.h>

struct drw;
struct drwsurf;
struct kbd;

void drw_init(struct drw *d, const char *fc_pattern, struct wl_display *dpy,
	void *iface);
void drwsurf_init(struct drw *d, struct drwsurf *ds, struct wl_surface *surf);
void drwsurf_resize(struct drwsurf *ds, uint32_t w, uint32_t h);
void drwsurf_flip(struct drwsurf *ds);

typedef union {
	uint8_t bgra[4];
	uint32_t color;
} Color;

void
drw_fill_rectangle(struct drwsurf *d, Color color, uint32_t x, uint32_t y,
	uint32_t w, uint32_t h);

void
drw_draw_text(struct drwsurf *d, Color color,
	uint32_t x, uint32_t y,
	uint32_t w, uint32_t h,
	const char *label);

uint32_t
setup_buffer(struct drwsurf *drwsurf);

struct drw {
	struct wl_shm *shm;
	PangoFontDescription *font_description;
};

struct drwsurf {
	uint32_t w, h, s;
	bool dirty;

	struct drw *ctx;
	struct wl_surface *surf;
	struct wl_buffer *buf;
	struct wl_shm *shm;
	unsigned char *pool_data;

	cairo_t *cairo;
	PangoLayout *layout;

	struct wl_callback *cb;
};

#endif
