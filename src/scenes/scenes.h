#pragma once

#include <stdint.h>
#include <stdbool.h>

struct Scene
{
    const char *name;
    bool (*initialize)();
    void (*update)(int64_t delta_ns);
    void (*draw)();
    void (*deinitialize)();
};

bool run_scenes();
