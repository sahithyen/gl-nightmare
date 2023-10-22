// SPDX-FileCopyrightText: 2023 Sahithyen Kanaganayagam <mail@sahithyen.com>
// SPDX-License-Identifier: MIT

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
