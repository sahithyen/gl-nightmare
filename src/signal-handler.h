// SPDX-FileCopyrightText: 2023 Sahithyen Kanaganayagam <mail@sahithyen.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <signal.h>

extern volatile sig_atomic_t sigint_triggered;

void initialize_signal_handler();
