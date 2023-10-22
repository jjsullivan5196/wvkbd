/*
 * To determine what the user drew use the following approach:
 * Determine the angle for all vectors that exeed the minimum length either from
 * the origin or the furthest point. If they are all within the same 45° area,
 * we have a line otherwise we have a circle. Save the lenght and position of
 * the longest distance. If the current distance is shorter than the furthest
 * distance, calc the distance between current and furthest. If that vector is
 * longer than the minimum distance and the direction is opposite to the
 * original direction we have back-forth. If the direction is sth. else, we have
 * a circle.
 */

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>


#include "keyboard.h"
#include "motion_key.h"

static int cur_x = -1, cur_y = -1;
static bool cur_press = false;

#define COS_22_5_DEG 0.9238795325112867
#define COS_67_5_DEG 0.3826834323650898
#define COS_112_5_DEG -0.3826834323650898
#define COS_157_5_DEG -0.9238795325112867

enum kbd_shape {
    UNDETERMINED_SHAPE = 0,
    TAP,
    LINE,
    BACK_FORTH,
    CIRCLE,
};

enum swipe_dir {
    UNDEFINED_DIR = -1,
    NORTH,
    NORTH_EAST,
    EAST,
    SOUTH_EAST,
    SOUTH,
    SOUTH_WEST,
    WEST,
    NORTH_WEST,
};

static enum kbd_shape curr_shape;

struct point {
    int x;
    int y;
};

struct vector {
    int x;
    int y;
    double cached_len;
};

/*
 * Based on the algorithm above, we need P0 and Pn
 */
static struct point p0;
static struct point pf;

static enum swipe_dir line_dir;

static double max_dist;

extern struct kbd keyboard;

#define MIN_LINE_LEN (15)
#define MIN_LEN_SQUARED (MIN_LINE_LEN * MIN_LINE_LEN)

static void
calc_vec_len(struct vector *v)
{
    v->cached_len = sqrt(v->x * v->x + v->y * v->y);
}

static enum swipe_dir
calc_dir(struct vector v)
{
    double cos_phi = (v.x * 0 + v.y * -1) / (v.cached_len);

    if (cos_phi > COS_22_5_DEG)
        return NORTH;
    if (cos_phi > COS_67_5_DEG)
        return v.x > 0 ? NORTH_EAST : NORTH_WEST;
    if (cos_phi > COS_112_5_DEG)
        return v.x > 0 ? EAST : WEST;
    if (cos_phi > COS_157_5_DEG)
        return v.x > 0 ? SOUTH_EAST : SOUTH_WEST;
    // greater than 157.5°
    return SOUTH;
}

static bool
is_opposite(enum swipe_dir d1, enum swipe_dir d2)
{
    return (d1 == ((d2 + 4) % 8));
}

static void
kbd_add_coord(struct point p)
{
    int dx, dy, len_squared;
    enum swipe_dir dir, dirf;

    /*
     * Once, we have determined the type of shape (back-and-forth or circle),
     * stick to it.
     */
    if (curr_shape) {
        return;
    }

    dir = dirf = UNDEFINED_DIR;

    /* Determine the angle for all vectors that exeed the minimum length either
     * from the origin or the furthest point. If they are all within the same
     * 45° area, we have a line otherwise we have a circle. Save the lenght and
     * position of the longest distance. If the current distance is shorter than
     * the furthest distance, calc the distance between current and furthest. If
     * that vector is longer than the minimum distance and the direction is
     * opposite to the original direction we have back-forth. If the direction
     * is sth. else, we have a circle.
     */
    // determine length of vector
    dx = p.x - p0.x;
    dy = p.y - p0.y;
    len_squared = dx * dx + dy * dy;

    if (len_squared >= MIN_LEN_SQUARED) {
        struct vector v;

        v.x = dx;
        v.y = dy;
        v.cached_len = sqrt(len_squared);
        dir = calc_dir(v);

        if (line_dir == UNDEFINED_DIR) {
            line_dir = dir;
        }

        if (v.cached_len > max_dist) {
            max_dist = v.cached_len;
            if (v.cached_len > MIN_LINE_LEN) {
                pf = p;
            }
        }
    }
    if (pf.x >= 0) {
        struct vector vf;

        vf.x = p.x - pf.x;
        vf.y = p.y - pf.y;
        calc_vec_len(&vf);

        if (vf.cached_len > MIN_LINE_LEN) {
            dirf = calc_dir(vf);
        }
    }

    if ((dirf != UNDEFINED_DIR) && (is_opposite(line_dir, dirf))) {
        curr_shape = BACK_FORTH;
    } else if ((dir != UNDEFINED_DIR) && (dir != line_dir)) {
        curr_shape = CIRCLE;
    }
}

static void
swp_start_swp(int x, int y)
{
    p0.x = x;
    p0.y = y;
    curr_shape = UNDETERMINED_SHAPE;
    line_dir = UNDEFINED_DIR;
    max_dist = 0.0;
    pf.x = -1;
    pf.y = -1;
    alarm(1); // start timer for long taps
}

static void
swp_determine_shape()
{
    if (!curr_shape) {
        if (max_dist < MIN_LINE_LEN)
            curr_shape = TAP;
        else
            curr_shape = LINE;
    }
}

static struct key*
mk_get_key_from_dir(struct key* key, enum swipe_dir dir)
{
    switch(dir) {
    case UNDEFINED_DIR:
        return NULL;
    case NORTH:
        return key->north;
    case NORTH_EAST:
        return key->north_east;
    case EAST:
        return key->east;
    case SOUTH_EAST:
        return key->south_east;
    case SOUTH:
        return key->south;
    case SOUTH_WEST:
        return key->south_west;
    case WEST:
        return key->west;
    case NORTH_WEST:
        return key->north_west;
    }
    return NULL;
}

static void
swp_handle_shape(uint32_t time)
{
    struct key *next_key = kbd_get_key(&keyboard, p0.x, p0.y);

    if (!next_key)
        goto out;

    switch (curr_shape) {
    case UNDETERMINED_SHAPE:
        printf("Undetermined\n");
        break;
    case TAP:
        kbd_press_key(&keyboard, next_key, time);
        kbd_release_key(&keyboard, time);
        break;
    case CIRCLE:
        keyboard.mods ^= Shift;
        kbd_press_key(&keyboard, next_key, time);
        kbd_release_key(&keyboard, time);
        break;
    case LINE:
        next_key = mk_get_key_from_dir(next_key, line_dir);
        if (next_key) {
            kbd_press_key(&keyboard, next_key, time);
            kbd_release_key(&keyboard, time);
        }
        break;
    case BACK_FORTH:
        next_key = mk_get_key_from_dir(next_key, line_dir);
        if (next_key) {
            keyboard.mods ^= Shift;
            kbd_press_key(&keyboard, next_key, time);
            kbd_release_key(&keyboard, time);
        }
        break;
    }

out:
    p0.x = -1;
    p0.y = -1;
}

void
timer_mk() {
    // Special handling for long taps
    if (!curr_shape && max_dist < MIN_LINE_LEN) {
        struct key *next_key = kbd_get_key(&keyboard, p0.x, p0.y);
        if (!next_key)
            return;
        next_key = next_key->long_tap;
        if (next_key) {
            kbd_press_key(&keyboard, next_key, /*time*/ 0);
            kbd_release_key(&keyboard, /*time*/ 0);
            p0.x = -1;
            p0.y = -1;
        }
    }
}

void
wl_touch_down_mk(void *data, struct wl_touch *wl_touch, uint32_t serial,
                 uint32_t time, struct wl_surface *surface, int32_t id,
                 wl_fixed_t x, wl_fixed_t y)
{
    int ix, iy;

    ix = wl_fixed_to_int(x);
    iy = wl_fixed_to_int(y);
    swp_start_swp(ix, iy);
}

void
wl_touch_up_mk(void *data, struct wl_touch *wl_touch, uint32_t serial,
               uint32_t time, int32_t id)
{
    alarm(0); // cancel timer
    // This happens on long tap
    if (p0.x == -1)
        return;
    swp_determine_shape();
    swp_handle_shape(time);
}

void
wl_touch_motion_mk(void *data, struct wl_touch *wl_touch, uint32_t time,
                   int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    struct point p;

    // This happens on long tap
    if (p0.x == -1)
        return;

    p.x = wl_fixed_to_int(x);
    p.y = wl_fixed_to_int(y);

    kbd_add_coord(p);
}

void
wl_pointer_leave_mk(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
                    struct wl_surface *surface)
{
    cur_x = cur_y = -1;
}

void
wl_pointer_motion_mk(void *data, struct wl_pointer *wl_pointer, uint32_t time,
                     wl_fixed_t surface_x, wl_fixed_t surface_y)
{
    cur_x = wl_fixed_to_int(surface_x);
    cur_y = wl_fixed_to_int(surface_y);

    if (cur_press) {
        struct point p;

        // This happens on long tap
        if (p0.x == -1)
            return;

        p.x = cur_x;
        p.y = cur_y;

        kbd_add_coord(p);
    }
}

void
wl_pointer_button_mk(void *data, struct wl_pointer *wl_pointer, uint32_t serial,
                     uint32_t time, uint32_t button, uint32_t state)
{
    cur_press = state == WL_POINTER_BUTTON_STATE_PRESSED;
    if (cur_press && cur_x >= 0 && cur_y >= 0) {
        swp_start_swp(cur_x, cur_y);
    } else if (!cur_press && p0.x >= 0 && p0.y >= 0) {
        alarm(0); // cancel timer
        swp_determine_shape();
        swp_handle_shape(time);
    }
}
