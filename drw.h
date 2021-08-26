#ifndef __DRW_H
#define __DRW_H

#include <pango/pangocairo.h>
#include <stdbool.h>

struct drw {
	struct wl_shm *shm;
	PangoFontDescription *font_description;
};
struct drwsurf {
	uint32_t width, height, scale, size;

	struct drw *ctx;
	struct wl_surface *surf;
	struct wl_buffer *buf;
	struct wl_shm *shm;
	struct wl_callback *cb;
	unsigned char *pool_data;

	cairo_t *cairo;
	PangoLayout *layout;
};
struct kbd;

void drwsurf_resize(struct drwsurf *ds, uint32_t w, uint32_t h, uint32_t s);
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

#endif
