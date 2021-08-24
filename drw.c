#include <wayland-client.h>
#include <sys/mman.h>
#include <unistd.h>

#include "drw.h"
#include "shm_open.h"

void
drw_init(struct drw *d, const char *fc_pattern, void *shm) {
	d->shm = shm;
	d->font_description = pango_font_description_from_string(fc_pattern);
}

void
drwsurf_init(struct drw *d, struct drwsurf *ds, struct wl_surface *surf) {
	ds->ctx = d;
	ds->surf = surf;
}

void
drwsurf_resize(struct drwsurf *ds, uint32_t w, uint32_t h, uint32_t s) {
	if (ds->buf) {
		munmap(ds->pool_data, ds->size);
		wl_buffer_destroy(ds->buf);
		ds->buf = NULL;
	}

	ds->scale = s;
	ds->width = w * s;
	ds->height = h * s;

	setup_buffer(ds);
}

static void surface_frame_callback(void *data, struct wl_callback *cb,
                                   uint32_t time);

static struct wl_callback_listener frame_listener = {
    .done = surface_frame_callback
};

void
drwsurf_flip(struct drwsurf *ds) {
	ds->cb = wl_surface_frame(ds->surf);
	wl_callback_add_listener(ds->cb, &frame_listener, (void *)ds);

	if (ds->dirty) {
		wl_surface_damage(ds->surf, 0, 0, ds->width, ds->height);
		ds->dirty = false;
	}

	wl_surface_attach(ds->surf, ds->buf, 0, 0);
	wl_surface_set_buffer_scale(ds->surf, ds->scale);
	wl_surface_commit(ds->surf);
}

void
surface_frame_callback(void *data, struct wl_callback *cb, uint32_t time) {
	struct drwsurf *ds = (struct drwsurf *)data;
	wl_callback_destroy(cb);
	ds->cb = NULL;

	drwsurf_flip(ds);
}

void
drw_draw_text(struct drwsurf *d, Color color,
	uint32_t x, uint32_t y,
	uint32_t w, uint32_t h,
	const char *label) {

	cairo_save(d->cairo);

    cairo_set_source_rgba (
		d->cairo,
		color.bgra[2] / (double)255,
		color.bgra[1] / (double)255,
		color.bgra[0] / (double)255,
		color.bgra[3] / (double)255
    );
	cairo_move_to(d->cairo, x + (double)w / 2.0, y + (double)h / 2.0);

	pango_layout_set_text(d->layout, label, -1);

	int width, height;
	pango_layout_get_size(d->layout, &width, &height);

	cairo_rel_move_to(d->cairo, - ((double)width / PANGO_SCALE) / 2, - ((double)height / PANGO_SCALE) / 2);
	pango_cairo_show_layout(d->cairo, d->layout);
	cairo_restore(d->cairo);
}

void
drw_fill_rectangle(struct drwsurf *d, Color color, uint32_t x, uint32_t y,
	uint32_t w, uint32_t h) {
	cairo_save(d->cairo);

	cairo_set_operator(d->cairo, CAIRO_OPERATOR_SOURCE);

	cairo_rectangle(d->cairo, x, y, w, h);
	cairo_set_source_rgba(
		d->cairo,
		color.bgra[2] / (double)255,
		color.bgra[1] / (double)255,
		color.bgra[0] / (double)255,
		color.bgra[3] / (double)255
	);
	cairo_fill(d->cairo);

	cairo_restore(d->cairo);
}

uint32_t
setup_buffer(struct drwsurf *drwsurf)
{
	int stride = drwsurf->width * 4;
	drwsurf->size = stride * drwsurf->height;

	int fd = allocate_shm_file(drwsurf->size);
	if (fd == -1) {
		return 1;
	}

	drwsurf->pool_data = mmap(NULL, drwsurf->size,
	PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if (drwsurf->pool_data == MAP_FAILED) {
		close(fd);
		return 1;
	}

	struct wl_shm_pool *pool = wl_shm_create_pool(drwsurf->ctx->shm, fd, drwsurf->size);
	drwsurf->buf = wl_shm_pool_create_buffer(pool, 0,
		drwsurf->width, drwsurf->height, stride, WL_SHM_FORMAT_ARGB8888);
	wl_shm_pool_destroy(pool);
	close(fd);

	cairo_surface_t *s = cairo_image_surface_create_for_data(drwsurf->pool_data,
		CAIRO_FORMAT_ARGB32,
	drwsurf->width, drwsurf->height, stride);

	drwsurf->cairo = cairo_create(s);
	cairo_scale(drwsurf->cairo, drwsurf->scale, drwsurf->scale);
	drwsurf->layout = pango_cairo_create_layout(drwsurf->cairo);
	pango_layout_set_font_description(drwsurf->layout, drwsurf->ctx->font_description);
	cairo_save(drwsurf->cairo);

	return 0;
}

