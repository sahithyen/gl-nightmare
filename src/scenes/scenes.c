// SPDX-FileCopyrightText: 2023 Sahithyen Kanaganayagam <mail@sahithyen.com>
// SPDX-License-Identifier: MIT

#include "scenes.h"

#include <time.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "floating-graph.h"
#include "fixed-graph.h"
#include "signal-handler.h"
#include "egl.h"

size_t scenes_count = 2;
struct Scene *scenes[] = {
    &floating_graph_scene,
    &fixed_graph_scene,
};

static bool run_scene(struct Scene *scene);

bool run_scenes()
{
    for (int i = 0; i < scenes_count; i++)
    {
        if (!run_scene(scenes[i]))
        {
            return false;
        }
        else if (sigint_triggered)
        {
            return true;
        }
    }

    return true;
}

static bool run_scene(struct Scene *scene)
{
    print("run scene '%s'\n", scene->name);

    if (!scene->initialize())
    {
        print_error("Failed to initialize EGL\n");
        return false;
    }

    struct timespec started;
    struct timespec last;
    uint64_t frames = 0;
    clock_gettime(CLOCK_MONOTONIC, &started);
    last = started;

    // Mainloop
    while (difftimespec_ns(last, started) < 15 * SEC_IN_NS)
    {
        frames++;

        // Check SIGINT
        if (sigint_triggered)
        {
            goto finish;
        }

        // Run any EGL specific mainloop tasks
        egl_loop_step();

        // Calculate time delta
        struct timespec current;
        clock_gettime(CLOCK_MONOTONIC, &current);
        int64_t delta_ns = difftimespec_ns(current, last);
        last = current;

        // Update scene
        scene->update(delta_ns);

        // Draw scene
        scene->draw();
        eglSwapBuffers(egl_display, egl_surface);
    }

    struct timespec stopped;
    clock_gettime(CLOCK_MONOTONIC, &stopped);
    double elapsed_time = ((double)difftimespec_ns(stopped, started)) / 1e9;
    double fps = ((double)frames) / elapsed_time;

    print("Average FPS = %f\n", fps);
    print("---\n\n");

finish:
    scene->deinitialize();
    return true;
}