#ifndef __DRW_H
#define __DRW_H

struct drw;
struct drwsurf;

void drw_init(struct drw *d, const char *fc_pattern, struct wl_display *dpy,
              void *iface);
void drwsurf_init(struct drw *d, struct drwsurf *ds, struct wl_surface *surf);
void drwsurf_resize(struct drwsurf *ds, uint32_t w, uint32_t h);
void drwsurf_flip(struct drwsurf *ds);

struct drw {
	struct wld_context *wld;
	struct wld_font_context *fctx;
	struct wld_font *font;
};

struct drwsurf {
	uint32_t w, h;
	bool dirty;

	struct drw *ctx;
	struct wl_surface *surf;
	struct wld_renderer *render;
	struct wld_buffer *buf;
	struct wl_buffer *ref;

	struct wl_callback *cb;
};

typedef union {
	uint8_t bgra[4];
	uint32_t color;
} Color;

#endif
