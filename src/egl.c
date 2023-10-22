#include "egl.h"

#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "common.h"

/**
 * EGL/X11
 */

Display *x11_display = NULL;
Window x11_window;

int egl_major = -1;
int egl_minor = -1;
EGLDisplay egl_display;
EGLSurface egl_surface;

int screen_width = 0;
int screen_height = 0;

bool initialize_egl()
{
    // Open X11 display
    x11_display = XOpenDisplay(NULL);

    if (!x11_display)
    {
        print_error("Could not get X11 display\n");
        return false;
    }

    // Get EGL display
    char const *egl_extensions = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);

    if (!egl_extensions)
    {
        print_error("Could not query EGL extensions\n");
        return false;
    }

    if (!strstr(egl_extensions, "EGL_EXT_platform_base"))
    {
        print_error("On this device EGL does not support the required extension EGL_EXT_platform_base\n");
        return false;
    }

    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    egl_display = eglGetPlatformDisplayEXT(EGL_PLATFORM_X11_KHR, (void *)x11_display, NULL);

    // Initialize EGL
    EGLBoolean egl_success = eglInitialize(egl_display, &egl_major, &egl_minor);

    if (!egl_success)
    {
        print_error("Could not initialize EGL\n");
        return false;
    }

    // Choose framebuffer configuration
#ifdef NIGHTMARE_USE_GLES1
    EGLint renderable_type = EGL_OPENGL_ES_BIT;
#elif defined NIGHTMARE_USE_GLES2
    EGLint renderable_type = EGL_OPENGL_ES2_BIT;
#endif

    const EGLint attribute_list[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_RENDERABLE_TYPE, renderable_type,
        EGL_NONE};

    // Get number of matching framebuffer configurations
    EGLint found_configs = 0;
    egl_success = eglChooseConfig(egl_display, attribute_list, NULL, 0, &found_configs);
    if (!egl_success || found_configs == 0)
    {
        print_error("Failed to query framebuffer configurations\n");
        return false;
    }
    else if (found_configs == 0)
    {
        print_error("No supported framebuffer configurations found\n");
        return false;
    }

    // http://directx.com/2014/06/egl-understanding-eglchooseconfig-then-ignoring-it/

    // Get matching framebuffer configurations
    EGLint config_count = found_configs;
    EGLConfig *configs = (EGLConfig *)calloc(sizeof(EGLConfig), (size_t)config_count);
    egl_success = eglChooseConfig(egl_display, attribute_list, configs, config_count, &found_configs);
    if (!egl_success)
    {
        free(configs);
        print_error("Could not retrieve matched framebuffer configurations\n");
        return false;
    }
    else if (config_count != found_configs)
    {
        free(configs);
        print_error("Different counts of supported framebuffer configurations after back to back call\n");
        return false;
    }

    // Choose framebuffer configuration
    // TODO: currently selecting the first result. selecting the one with the least capabilities would be better
    EGLConfig egl_config = configs[0];
    EGLint config_id = 0;
    if (!eglGetConfigAttrib(egl_display, egl_config, EGL_CONFIG_ID, &config_id))
    {
        free(configs);
        print_error("Could not retrieve id from framebuffer configuration\n");
        return false;
    }

    EGLint native_id;
    if (!eglGetConfigAttrib(egl_display, egl_config, EGL_NATIVE_VISUAL_ID, &native_id))
    {
        free(configs);
        print_error("Could not retrieve X11 id from framebuffer configuration\n");
        return false;
    }

    free(configs);

    // Get screen size
    XWindowAttributes root_window_attributes;
    Window root_window = RootWindow(x11_display, DefaultScreen(x11_display));
    Status x11_status = XGetWindowAttributes(x11_display, root_window, &root_window_attributes);
    screen_width = root_window_attributes.width;
    screen_height = root_window_attributes.height;

    if (!x11_status)
    {
        print_error("Could not retrieve screen size via X11\n");
        return false;
    }

    // Get XVisualInfo
    XVisualInfo visual_info_template;
    visual_info_template.visualid = native_id;
    XVisualInfo *visual_info = NULL;
    int visuals_count;
    visual_info = XGetVisualInfo(x11_display, VisualIDMask, &visual_info_template, &visuals_count);

    if (!visual_info)
    {
        print_error("Could not retrieve X11 visual info\n");
        return false;
    }

    // Create window
    XSetWindowAttributes set_window_attributes;

    set_window_attributes.background_pixel = 0;
    set_window_attributes.border_pixel = 0;
    set_window_attributes.colormap = XCreateColormap(x11_display, root_window, visual_info->visual, AllocNone);
    set_window_attributes.event_mask = 0;
    unsigned long mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

    x11_window = XCreateWindow(
        x11_display, root_window,
        0, // x
        0, // y
        screen_width,
        screen_height,
        0, // Border width
        visual_info->depth,
        InputOutput,
        visual_info->visual,
        mask,
        &set_window_attributes);

    XFree(visual_info);

    if (!x11_window)
    {
        print_error("Could not create X11 window\n");
        return false;
    }

    // Full window
    Atom fullscreen_atom = XInternAtom(x11_display, "_NET_WM_STATE_FULLSCREEN", True);
    if (fullscreen_atom == None)
    {
        print_error("Could not set EWMH Fullscreen hint\n");
        return false;
    }

    XChangeProperty(
        x11_display,
        x11_window,
        XInternAtom(x11_display, "_NET_WM_STATE", True),
        XA_ATOM,
        32,
        PropModeReplace,
        (unsigned char *)&fullscreen_atom,
        1);

    // Set window name
    XStoreName(x11_display, x11_window, "nightmare benchmark");

    // Gracefulle handle the window delete event
    Atom wm_delete = XInternAtom(x11_display, "WM_DELETE_WINDOW", True);
    XSetWMProtocols(x11_display, x11_window, &wm_delete, 1);

    // Create EGL surface
    egl_surface = eglCreateWindowSurface(egl_display, egl_config, (EGLNativeWindowType)x11_window, NULL);
    if (!egl_surface)
    {
        print_error("Could not create EGL surface (error code: %x)\n", eglGetError());
        return false;
    }

    // Create context
#ifdef NIGHTMARE_USE_GLES1
    EGLint context_attributes[] = {
        EGL_CONTEXT_MAJOR_VERSION, 1,
        EGL_NONE};
#elif defined NIGHTMARE_USE_GLES2
    EGLint context_attributes[] = {
        EGL_CONTEXT_MAJOR_VERSION, 2,
        EGL_NONE};
#endif

    EGLContext egl_context = eglCreateContext(egl_display, egl_config,
                                              EGL_NO_CONTEXT, context_attributes);

    // Make current
    egl_success = eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
    if (!egl_success)
    {
        print_error("Could not set EGL context as current one (error code: %x)\n", eglGetError());
        return false;
    }

    // Set zero time swapping
    egl_success = eglSwapInterval(egl_display, 0);
    if (!egl_success)
    {
        print_error("Could not set zero time swapping (error code: %x)\n", eglGetError());
        return false;
    }

    XMapWindow(x11_display, x11_window);

    return true;
}

void cleanup_egl()
{
    eglTerminate(egl_display);

    if (x11_display)
    {
        XDestroyWindow(x11_display, x11_window);

        XCloseDisplay(x11_display);
        x11_display = NULL;
    }
}

void egl_loop_step()
{
    XEvent event;
    if (XPending(x11_display))
    {
        if (XCheckWindowEvent(x11_display, x11_window, 0, &event))
        {
            // Should never be reached as all events are masked
            print("received x11 event: %i\n", event.type);
        }
    }
}
