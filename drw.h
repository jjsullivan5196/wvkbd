#ifndef __DRW_H
#define __DRW_H

#include <pango/pangocairo.h>
#include <stdbool.h>

struct drw {
	struct wl_shm *shm;
};
struct drwsurf {
	uint32_t width, height, size;
	double scale;

	struct drw *ctx;
	struct wl_surface *surf;
	struct wl_buffer *buf;
	struct wl_shm *shm;
	unsigned char *pool_data;

	cairo_t *cairo;
	PangoLayout *layout;
};
struct kbd;

void drwsurf_resize(struct drwsurf *ds, uint32_t w, uint32_t h, double s);
void drwsurf_flip(struct drwsurf *ds);

typedef union {
	uint8_t bgra[4];
	uint32_t color;
} Color;

void drw_do_clear(struct drwsurf *d, uint32_t x, uint32_t y,
                      uint32_t w, uint32_t h);
void drw_do_rectangle(struct drwsurf *d, Color color, uint32_t x, uint32_t y,
                      uint32_t w, uint32_t h, bool fill, int rounding);
void drw_fill_rectangle(struct drwsurf *d, Color color, uint32_t x, uint32_t y,
                        uint32_t w, uint32_t h, int rounding);
void drw_over_rectangle(struct drwsurf *d, Color color, uint32_t x, uint32_t y,
                        uint32_t w, uint32_t h, int rounding);

void drw_draw_text(struct drwsurf *d, Color color, uint32_t x, uint32_t y,
                   uint32_t w, uint32_t h, uint32_t b, const char *label,
                   PangoFontDescription *font_description);

uint32_t setup_buffer(struct drwsurf *drwsurf);

#endif
