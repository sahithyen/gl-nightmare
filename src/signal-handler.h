#pragma once

#include <signal.h>

extern volatile sig_atomic_t sigint_triggered;

void initialize_signal_handler();
