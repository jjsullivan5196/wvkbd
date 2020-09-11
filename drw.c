#include "wld/wayland.h"
#include "wld/wld.h"
#include <wayland-client.h>

#include "drw.h"

void
drw_init(struct drw *d, const char *fc_pattern, struct wl_display *dpy,
         void *iface) {
	d->wld = wld_wayland_create_context(dpy, WLD_ANY, iface);
	d->fctx = wld_font_create_context();
	d->font = wld_font_open_name(d->fctx, fc_pattern);
}

void
drwsurf_init(struct drw *d, struct drwsurf *ds, struct wl_surface *surf) {
	ds->ctx = d;
	ds->surf = surf;
	ds->render = wld_create_renderer(d->wld);
}

void
drwsurf_resize(struct drwsurf *ds, uint32_t w, uint32_t h) {
	union wld_object obj;

	if (ds->buf) {
		wld_buffer_unreference(ds->buf);
		ds->buf = NULL;
		ds->ref = NULL;
	}

	ds->w = w;
	ds->h = h;

	ds->buf = wld_create_buffer(ds->ctx->wld, w, h, WLD_FORMAT_ARGB8888, 0);
	wld_set_target_buffer(ds->render, ds->buf);

	wld_export(ds->buf, WLD_WAYLAND_OBJECT_BUFFER, &obj);
	ds->ref = obj.ptr;
}

static void surface_frame_callback(void *data, struct wl_callback *cb,
                                   uint32_t time);

static struct wl_callback_listener frame_listener = {.done =
                                                       surface_frame_callback};

void
drwsurf_flip(struct drwsurf *ds) {
	ds->cb = wl_surface_frame(ds->surf);
	wl_callback_add_listener(ds->cb, &frame_listener, (void *)ds);

	if (ds->dirty) {
		wl_surface_damage(ds->surf, 0, 0, ds->w, ds->h);
		wld_flush(ds->render);
		wld_set_target_buffer(ds->render, ds->buf);
		ds->dirty = false;
	}

	wl_surface_attach(ds->surf, ds->ref, 0, 0);
	wl_surface_commit(ds->surf);
}

void
surface_frame_callback(void *data, struct wl_callback *cb, uint32_t time) {
	struct drwsurf *ds = (struct drwsurf *)data;
	wl_callback_destroy(cb);
	ds->cb = NULL;

	drwsurf_flip(ds);
}
