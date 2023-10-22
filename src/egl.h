// SPDX-FileCopyrightText: 2023 Sahithyen Kanaganayagam <mail@sahithyen.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <stdbool.h>
#include <X11/Xlib.h>
#include <EGL/egl.h>

extern Display *x11_display;
extern Window x11_window;

extern int egl_major;
extern int egl_minor;
extern EGLDisplay egl_display;
extern EGLSurface egl_surface;

extern int screen_width;
extern int screen_height;

bool initialize_egl();
void cleanup_egl();

void egl_loop_step();
