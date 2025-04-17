#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>

#include "drw.h"
#include "shm_open.h"
#include "math.h"

void drwbuf_handle_release(void *data, struct wl_buffer *wl_buffer) {
    struct drwbuf *db = data;
    db->released = true;
};

const struct wl_buffer_listener buffer_listener = {
    .release = drwbuf_handle_release
};

void drwsurf_handle_frame_cb(void* data, struct wl_callback* callback,
    uint32_t time)
{
    struct drwsurf *ds = data;
    wl_callback_destroy(ds->frame_cb);
    ds->frame_cb = NULL;

    cairo_rectangle_int_t r = {0};
    for (int i = 0; i < cairo_region_num_rectangles(ds->back_buffer->damage); i++) {
        cairo_region_get_rectangle(ds->back_buffer->damage, i, &r);
        wl_surface_damage(ds->surf, r.x, r.y, r.width, r.height);
    };
    cairo_region_subtract(ds->display_buffer->damage, ds->display_buffer->damage);

    drwsurf_attach(ds);
}

const struct wl_callback_listener frame_listener = {
    .done = drwsurf_handle_frame_cb
};

void drwsurf_register_frame_cb(struct drwsurf *ds)
{
    if (ds->frame_cb)
        return;
    if (!ds->attached)
        return;
    ds->frame_cb = wl_surface_frame(ds->surf);
    wl_callback_add_listener(ds->frame_cb, &frame_listener, ds);
    wl_surface_commit(ds->surf);
}

void drwsurf_damage(struct drwsurf *ds, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    cairo_rectangle_int_t rect = { x, y, w, h };
    cairo_region_union_rectangle(ds->back_buffer->damage, &rect);
    cairo_region_union_rectangle(ds->back_buffer->backport_damage, &rect);
    drwsurf_register_frame_cb(ds);
}

void
drwsurf_resize(struct drwsurf *ds, uint32_t w, uint32_t h, double s)
{
    ds->scale = s;
    ds->width = ceil(w * s);
    ds->height = ceil(h * s);

    setup_buffer(ds, ds->back_buffer);
    setup_buffer(ds, ds->display_buffer);
}

void
drwsurf_backport(struct drwsurf *ds)
{
    cairo_save(ds->back_buffer->cairo);

    cairo_scale(ds->back_buffer->cairo, 1/ds->scale, 1/ds->scale);
    cairo_set_operator(ds->back_buffer->cairo, CAIRO_OPERATOR_SOURCE);

    cairo_rectangle_int_t r = {0};
    for (int i = 0; i < cairo_region_num_rectangles(ds->display_buffer->backport_damage); i++) {
        cairo_region_get_rectangle(ds->display_buffer->backport_damage, i, &r);

        cairo_set_source_surface(ds->back_buffer->cairo, ds->display_buffer->cairo_surf, 0, 0);
        cairo_rectangle(
            ds->back_buffer->cairo,
            r.x * ds->scale,
            r.y * ds->scale,
            r.width * ds->scale,
            r.height * ds->scale
        );
        cairo_fill(ds->back_buffer->cairo);
    };

    cairo_restore(ds->back_buffer->cairo);
    cairo_region_subtract(ds->display_buffer->backport_damage, ds->display_buffer->backport_damage);
}

void
drwsurf_attach(struct drwsurf *ds)
{
    wl_surface_attach(ds->surf, ds->back_buffer->buf, 0, 0);
    wl_surface_commit(ds->surf);
    ds->back_buffer->released = false;
    ds->attached = true;
}

void
drwsurf_flip(struct drwsurf *ds)
{
    if (ds->back_buffer->released)
        return;
    struct drwbuf *tmp = ds->back_buffer;
    ds->back_buffer = ds->display_buffer;
    ds->display_buffer = tmp;

    drwsurf_backport(ds);
}

void
drw_draw_text(struct drwsurf *ds, Color color, uint32_t x, uint32_t y,
              uint32_t w, uint32_t h, uint32_t b, const char *label,
              PangoFontDescription *font_description)
{
    drwsurf_flip(ds);
    struct drwbuf *d = ds->back_buffer;

    cairo_save(d->cairo);

    pango_layout_set_font_description(d->layout, font_description);

    cairo_set_source_rgba(
        d->cairo, color.bgra[2] / (double)255, color.bgra[1] / (double)255,
        color.bgra[0] / (double)255, color.bgra[3] / (double)255);
    cairo_move_to(d->cairo, x + w / 2, y + h / 2);

    pango_layout_set_text(d->layout, label, -1);
    pango_layout_set_width(d->layout, (w - (b * 2)) * PANGO_SCALE);
    pango_layout_set_height(d->layout, (h - (b * 2)) * PANGO_SCALE);

    int width, height;
    pango_layout_get_pixel_size(d->layout, &width, &height);

    cairo_rel_move_to(d->cairo, -width / 2, -height / 2);

    pango_cairo_show_layout(d->cairo, d->layout);
    cairo_restore(d->cairo);
}

void
drw_do_clear(struct drwsurf *ds, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    drwsurf_flip(ds);
    struct drwbuf *d = ds->back_buffer;

    cairo_save(d->cairo);

    cairo_set_operator(d->cairo, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle(d->cairo, x, y, w, h);
    cairo_fill(d->cairo);

    cairo_restore(d->cairo);
}

void
drw_do_rectangle(struct drwsurf *ds, Color color, uint32_t x, uint32_t y,
                 uint32_t w, uint32_t h, bool over, int rounding)
{
    drwsurf_flip(ds);
    struct drwbuf *d = ds->back_buffer;

    cairo_save(d->cairo);

    if (over) {
        cairo_set_operator(d->cairo, CAIRO_OPERATOR_OVER);
    } else {
        cairo_set_operator(d->cairo, CAIRO_OPERATOR_SOURCE);
    }

    if (rounding > 0) {
        double radius = rounding / 1.0;
        double degrees = M_PI / 180.0;

        cairo_new_sub_path (d->cairo);
        cairo_arc (d->cairo, x + w - radius, y + radius, radius, -90 * degrees, 0 * degrees);
        cairo_arc (d->cairo, x + w - radius, y + h - radius, radius, 0 * degrees, 90 * degrees);
        cairo_arc (d->cairo, x + radius, y + h - radius, radius, 90 * degrees, 180 * degrees);
        cairo_arc (d->cairo, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
        cairo_close_path (d->cairo);

        cairo_set_source_rgba(
          d->cairo, color.bgra[2] / (double)255, color.bgra[1] / (double)255, 
          color.bgra[0] / (double)255, color.bgra[3] / (double)255);
        cairo_fill (d->cairo);
        cairo_set_source_rgba(d->cairo, 0, 0, 0, 0.9);
        cairo_set_line_width(d->cairo, 1.0);
        cairo_stroke(d->cairo);

        cairo_restore(d->cairo);
    }
    else {
        cairo_rectangle(d->cairo, x, y, w, h);
        cairo_set_source_rgba(
            d->cairo, color.bgra[2] / (double)255, color.bgra[1] / (double)255,
            color.bgra[0] / (double)255, color.bgra[3] / (double)255);
        cairo_fill(d->cairo);

        cairo_restore(d->cairo);
    }
}

void
drw_fill_rectangle(struct drwsurf *d, Color color, uint32_t x, uint32_t y,
                   uint32_t w, uint32_t h, int rounding)
{
    drw_do_rectangle(d, color, x, y, w, h, false, rounding);
}

void
drw_over_rectangle(struct drwsurf *d, Color color, uint32_t x, uint32_t y,
                   uint32_t w, uint32_t h, int rounding)
{
    drw_do_rectangle(d, color, x, y, w, h, true, rounding);
}

uint32_t
setup_buffer(struct drwsurf *drwsurf, struct drwbuf *drwbuf)
{
    int prev_size = drwbuf->size;
    int stride = drwsurf->width * 4;
    drwbuf->size = stride * drwsurf->height;

    int fd = allocate_shm_file(drwbuf->size);
    if (fd == -1) {
        return 1;
    }

    if (drwbuf->pool_data)
        munmap(drwbuf->pool_data, prev_size);
    drwbuf->pool_data =
        mmap(NULL, drwbuf->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (drwbuf->pool_data == MAP_FAILED) {
        close(fd);
        return 1;
    }

    if (drwbuf->buf)
        wl_buffer_destroy(drwbuf->buf);
    struct wl_shm_pool *pool =
        wl_shm_create_pool(drwsurf->ctx->shm, fd, drwbuf->size);
    drwbuf->buf =
        wl_shm_pool_create_buffer(pool, 0, drwsurf->width, drwsurf->height,
                                  stride, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);
    wl_buffer_add_listener(drwbuf->buf, &buffer_listener, drwbuf);
    drwbuf->released = true;


    if (drwbuf->cairo_surf)
        cairo_surface_destroy(drwbuf->cairo_surf);
    drwbuf->cairo_surf = cairo_image_surface_create_for_data(
        drwbuf->pool_data, CAIRO_FORMAT_ARGB32, drwsurf->width,
        drwsurf->height, stride);

    if (drwbuf->damage)
        cairo_region_destroy(drwbuf->damage);
    drwbuf->damage = cairo_region_create();
    if (drwbuf->backport_damage)
        cairo_region_destroy(drwbuf->backport_damage);
    drwbuf->backport_damage = cairo_region_create();

    if (drwbuf->cairo)
        cairo_destroy(drwbuf->cairo);
    drwbuf->cairo = cairo_create(drwbuf->cairo_surf);
    cairo_scale(drwbuf->cairo, drwsurf->scale, drwsurf->scale);
    cairo_set_antialias(drwbuf->cairo, CAIRO_ANTIALIAS_NONE);
    drwbuf->layout = pango_cairo_create_layout(drwbuf->cairo);
    pango_layout_set_auto_dir(drwbuf->layout, false);
    cairo_save(drwbuf->cairo);

    return 0;
}
