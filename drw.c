#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client.h>

#include "drw.h"
#include "shm_open.h"
#include "math.h"

void
drwsurf_resize(struct drwsurf *ds, uint32_t w, uint32_t h, double s)
{
    if (ds->buf) {
        munmap(ds->pool_data, ds->size);
        wl_buffer_destroy(ds->buf);
        ds->buf = NULL;
    }

    ds->scale = s;
    ds->width = ceil(w * s);
    ds->height = ceil(h * s);

    setup_buffer(ds);
}

void
drwsurf_flip(struct drwsurf *ds)
{
    wl_surface_attach(ds->surf, ds->buf, 0, 0);
    wl_surface_commit(ds->surf);
}

void
drw_draw_text(struct drwsurf *d, Color color, uint32_t x, uint32_t y,
              uint32_t w, uint32_t h, uint32_t b, const char *label,
              PangoFontDescription *font_description)
{

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
drw_do_clear(struct drwsurf *d, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    cairo_save(d->cairo);

    cairo_set_operator(d->cairo, CAIRO_OPERATOR_CLEAR);
    cairo_rectangle(d->cairo, x, y, w, h);
    cairo_fill(d->cairo);

    cairo_restore(d->cairo);
}

void
drw_do_rectangle(struct drwsurf *d, Color color, uint32_t x, uint32_t y,
                 uint32_t w, uint32_t h, bool over, int rounding)
{
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
setup_buffer(struct drwsurf *drwsurf)
{
    int stride = drwsurf->width * 4;
    drwsurf->size = stride * drwsurf->height;

    int fd = allocate_shm_file(drwsurf->size);
    if (fd == -1) {
        return 1;
    }

    drwsurf->pool_data =
        mmap(NULL, drwsurf->size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (drwsurf->pool_data == MAP_FAILED) {
        close(fd);
        return 1;
    }

    struct wl_shm_pool *pool =
        wl_shm_create_pool(drwsurf->ctx->shm, fd, drwsurf->size);
    drwsurf->buf =
        wl_shm_pool_create_buffer(pool, 0, drwsurf->width, drwsurf->height,
                                  stride, WL_SHM_FORMAT_ARGB8888);
    wl_shm_pool_destroy(pool);
    close(fd);

    cairo_surface_t *s = cairo_image_surface_create_for_data(
        drwsurf->pool_data, CAIRO_FORMAT_ARGB32, drwsurf->width,
        drwsurf->height, stride);

    drwsurf->cairo = cairo_create(s);
    cairo_scale(drwsurf->cairo, drwsurf->scale, drwsurf->scale);
    cairo_set_antialias(drwsurf->cairo, CAIRO_ANTIALIAS_NONE);
    drwsurf->layout = pango_cairo_create_layout(drwsurf->cairo);
    pango_layout_set_auto_dir(drwsurf->layout, false);
    cairo_save(drwsurf->cairo);

    return 0;
}
