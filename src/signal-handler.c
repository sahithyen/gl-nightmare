// SPDX-FileCopyrightText: 2023 Sahithyen Kanaganayagam <mail@sahithyen.com>
// SPDX-License-Identifier: MIT

#include "signal-handler.h"

#include <signal.h>

volatile sig_atomic_t sigint_triggered = 0;

static void sig_handler(int _)
{
    (void)_;
    sigint_triggered = 1;
}

void initialize_signal_handler()
{
    signal(SIGINT, sig_handler);
}
