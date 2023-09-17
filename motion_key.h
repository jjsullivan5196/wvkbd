#ifndef MOTION_KEY_H_
#define MOTION_KEY_H_

#include <wayland-client-protocol.h>
#include <wayland-client.h>

void wl_touch_down_mk(void *data, struct wl_touch *wl_touch, uint32_t serial,
                      uint32_t time, struct wl_surface *surface, int32_t id,
                      wl_fixed_t x, wl_fixed_t y);
void wl_touch_up_mk(void *data, struct wl_touch *wl_touch, uint32_t serial,
                    uint32_t time, int32_t id);
void wl_touch_motion_mk(void *data, struct wl_touch *wl_touch, uint32_t time,
                        int32_t id, wl_fixed_t x, wl_fixed_t y);

void wl_pointer_leave_mk(void *data, struct wl_pointer *wl_pointer,
                         uint32_t serial, struct wl_surface *surface);
void wl_pointer_motion_mk(void *data, struct wl_pointer *wl_pointer,
                          uint32_t time, wl_fixed_t surface_x,
                          wl_fixed_t surface_y);
void wl_pointer_button_mk(void *data, struct wl_pointer *wl_pointer,
                          uint32_t serial, uint32_t time, uint32_t button,
                          uint32_t state);

#endif // MOTION_KEY_H_
