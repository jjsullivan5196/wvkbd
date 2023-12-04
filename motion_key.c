/*
 * For detecting motions/swipes on the keyboard, we need to determine if the
 * user used:
 *  - tap
 *  - long tap
 *  - drag
 *  - drag-and-return
 *  - circle
 *
 * The algorithm used here records all the points and determines the shape after
 * the user releases the touch / pointer (swp_determine_shape).
 *
 * A notable exception is the long tap. To detect a long tap we use an one
 * second alarm signal. We then evaluate the shape and if the shape is a tap,
 * then the user used a long tap.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>


#include "keyboard.h"
#include "motion_key.h"

static int cur_x = -1, cur_y = -1;
static bool cur_press = false;

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
 * Used for storing all the points during the touch/pointer motion.
 */
#define MAX_NUM_POINTS 100
static struct point points[MAX_NUM_POINTS];
static int num_points;

static enum swipe_dir line_dir;

extern struct kbd keyboard;

#define MIN_LINE_LEN (15)
#define MIN_LEN_SQUARED (MIN_LINE_LEN * MIN_LINE_LEN)

static void
calc_vec_len(struct vector *v)
{
    v->cached_len = sqrt(v->x * v->x + v->y * v->y);
}

/*
 * The following angles for detecting the direction of a motion. These are the
 * thresholds for the possible 8 directions.
 */
#define COS_22_5_DEG 0.9238795325112867
#define COS_67_5_DEG 0.3826834323650898
#define COS_112_5_DEG -0.3826834323650898
#define COS_157_5_DEG -0.9238795325112867

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
    // Array is full 🤷
    if (num_points >= MAX_NUM_POINTS) {
        printf("Array is full!\n");
        return;
    }

    points[num_points].x = p.x;
    points[num_points].y = p.y;
    num_points++;
}

static void
swp_start_swp(int x, int y)
{
    num_points = 1;
    points[0].x = x;
    points[0].y = y;
    alarm(1); // start timer for long taps
}

#define calc_dist_sq(p1, p2) ((p2.x - p1.x)*(p2.x-p1.x)+(p2.y-p1.y)*(p2.y-p1.y))

static double
calc_angle_cos(struct vector v1, struct vector v2) {
    return (v1.x * v2.x + v1.y * v2.y) / ((v1.cached_len)*(v2.cached_len));
}

static size_t
swp_determine_farthest_point(size_t ref_idx)
{
    int max_dist_sq = 0.0;
    size_t idx_pf = 0;
    for (size_t i = ref_idx + 1; i < num_points; i++) {
        int dist_sq = calc_dist_sq(points[ref_idx], points[i]);
        if ((dist_sq > MIN_LEN_SQUARED) && (dist_sq > max_dist_sq)) {
            max_dist_sq = dist_sq;
            idx_pf = i;
        }
    }
    return idx_pf;
}

static bool
swp_is_line(struct vector ref_vec, size_t ref_idx, size_t end_idx)
{
    for (size_t i = ref_idx + 1; i < end_idx; i++) {
        struct vector v0i; // vector from first point to point i
        v0i.x = points[i].x - points[ref_idx].x;
        v0i.y = points[i].y - points[ref_idx].y;
        calc_vec_len(&v0i);
        if (v0i.cached_len < MIN_LINE_LEN)
            continue;

        double v0i_v0f_cos = calc_angle_cos(v0i, ref_vec);
        if (v0i_v0f_cos < 0.0) {
            // not the same direction
            return false;
        }
    }
    return true;
}

static bool
swp_is_circle()
{
    struct point c = {0, 0};
    for (size_t i = 0; i < num_points; i++) {
        c.x += points[i].x;
        c.y += points[i].y;
    }
    c.x /= num_points;
    c.y /= num_points;
    int min_dist_sq = 100000;
    int max_dist_sq = 0;

    for (size_t i = 0; i < num_points; i++) {
        int dx, dy, dist_sq;
        dx = points[i].x - c.x;
        dy = points[i].y - c.y;
        dist_sq = dx * dx + dy * dy;
        min_dist_sq = MIN(dist_sq, min_dist_sq);
        max_dist_sq = MAX(dist_sq, max_dist_sq);
    }
    // If the minimal distance is greater than 1/4 of the maximal distance, we
    // cannot consider this a drag-with-return, it's a circle
    return ((min_dist_sq*16)>max_dist_sq);
}

/**
 * Approach for shape detection:
 * - Record all points
 * - Determine the farthest point from origin (pf)
 * - If it's < MIN_LINE_LEN: It's a trap! I mean: It's a tap! We're done here!
 * - Determine the direction of all vectors up to pf, skipping
 *   points that are not at least MIN_LINE_LEN away from the first point.
 * - If they are all approximately the same angle, the first part is a line,
 *   otherwise it's a circle and we're done.
 * - Of the remaining points, determine the farthest point from pf (pfr for
 *   farthest point return).
 * - If it's < MIN_LINE_LEN: The whole thing is a line. Determine direction
 *   and we're done!
 * - Determine the direction of all vectors after pf, skipping
 *   points that are not at least MIN_LINE_LEN away from the first point.
 * - If they are all approximately the same angle, the second part is also
 *   a line, otherwise it's a circle and we're done.
 * - Check if we the second line is in the opposite direction of the first
 *   line, if no: circle
 * - Determine the distance of each point to the centroid:
 *   If they all have (more or less) the same distance,
 *   it's a circle. Otherwise, it's drag-return.
 */
static enum kbd_shape
swp_determine_shape()
{
    size_t idx_pf = swp_determine_farthest_point(0);
    // - If there is no farthest point, it's a tap.
    if (!idx_pf)
        return TAP;


    struct vector v0f; // vector from first point to farthest point
    v0f.x = points[idx_pf].x - points[0].x;
    v0f.y = points[idx_pf].y - points[0].y;
    calc_vec_len(&v0f);


    /* - Determine the direction of all vectors up to pf, skipping
     *   points that are not at least MIN_LINE_LEN away from the first point.
     * - If they are all approximately the same angle, the first part is a line,
     *   otherwise it's a circle and we're done.
     */
    if (!swp_is_line(v0f, 0, idx_pf))
        return CIRCLE;

    /* - Of the remaining points, determine the farthest point from pf (pfr for
     *   farthest point return).
     */
    size_t idx_pfr = swp_determine_farthest_point(idx_pf);
    if (!idx_pfr) {
        /*
         * There are no valid points after the farthest point. It's a line.
         * Determine direction and we're done!
         */
        line_dir = calc_dir(v0f);
        return LINE;
    }

    struct vector vfr; // vector from farthest point (pf) to farthest point from farthest point (pfr)
    vfr.x = points[idx_pfr].x - points[idx_pf].x;
    vfr.y = points[idx_pfr].y - points[idx_pf].y;
    calc_vec_len(&vfr);

    /* - Determine the direction of all vectors after pf, skipping
     *   points that are not at least MIN_LINE_LEN away from the first point.
     * - If they are all approximately the same angle, the second part is also
     *   a line, otherwise it's a circle and we're done.
     */
    if (!swp_is_line(vfr, idx_pf, num_points-1))
        return CIRCLE;

    /* - Check if we the second line is in the opposite direction of the first
     *   line, if no: circle
     */
    line_dir = calc_dir(v0f);
    enum swipe_dir dir_r = calc_dir(vfr);
    if(!is_opposite(line_dir, dir_r))
        return CIRCLE;

    /* - Determine the distance of each point to the centroid:
     *   If they all have (more or less) the same distance,
     *   it's a circle. Otherwise, it's drag-return.
     */
    if (swp_is_circle())
        return CIRCLE;

    return BACK_FORTH;
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
swp_handle_shape(uint32_t time, enum kbd_shape shape)
{
    struct key *next_key = kbd_get_key(&keyboard, points[0].x, points[0].y);

    if (!next_key)
        goto out;

    switch (shape) {
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
    points[0].x = -1;
    points[0].y = -1;
}

void
timer_mk() {
    enum kbd_shape shape = swp_determine_shape();
    // Special handling for long taps
    if (shape == TAP) {
        struct key *next_key = kbd_get_key(&keyboard, points[0].x, points[0].y);
        if (!next_key)
            return;
        next_key = next_key->long_tap;
        if (next_key) {
            kbd_press_key(&keyboard, next_key, /*time*/ 0);
            kbd_release_key(&keyboard, /*time*/ 0);
            points[0].x = -1;
            points[0].y = -1;
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
    if (points[0].x == -1)
        return;
    enum kbd_shape shape = swp_determine_shape();
    swp_handle_shape(time, shape);
}

void
wl_touch_motion_mk(void *data, struct wl_touch *wl_touch, uint32_t time,
                   int32_t id, wl_fixed_t x, wl_fixed_t y)
{
    struct point p;

    // This happens on long tap
    if (points[0].x == -1)
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
        if (points[0].x == -1)
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
    } else if (!cur_press && points[0].x >= 0 && points[0].y >= 0) {
        alarm(0); // cancel timer
        enum kbd_shape shape = swp_determine_shape();
        swp_handle_shape(time, shape);
    }
}
