#include <stdint.h>
#include <stddef.h>
#include <math.h>

#include "common.h"

#define NUMBERS_COUNT 500
// Numbers generated at: https://www.random.org/decimal-fractions/?num=500&dec=2&col=1&format=html&rnd=new
static float numbers[NUMBERS_COUNT] = {
    0.93,
    0.39,
    0.25,
    0.65,
    0.44,
    0.85,
    0.25,
    0.46,
    0.00,
    0.35,
    0.35,
    0.54,
    0.94,
    0.48,
    0.41,
    0.91,
    0.48,
    0.57,
    0.99,
    0.33,
    0.49,
    0.55,
    0.58,
    0.34,
    0.81,
    0.31,
    0.82,
    0.84,
    0.80,
    0.22,
    0.35,
    0.20,
    0.61,
    0.28,
    0.26,
    0.93,
    0.40,
    0.97,
    0.92,
    0.58,
    0.36,
    0.52,
    0.03,
    0.25,
    0.96,
    0.04,
    0.31,
    0.14,
    0.30,
    0.90,
    0.03,
    0.37,
    0.67,
    0.40,
    0.06,
    0.87,
    0.54,
    0.17,
    0.06,
    0.85,
    0.60,
    0.63,
    0.12,
    0.08,
    0.36,
    0.68,
    0.16,
    0.37,
    0.07,
    0.31,
    0.98,
    0.42,
    0.06,
    0.62,
    0.31,
    0.36,
    0.96,
    0.24,
    0.90,
    0.02,
    0.13,
    0.81,
    0.66,
    0.11,
    0.05,
    0.18,
    0.95,
    0.86,
    0.33,
    0.23,
    0.39,
    0.14,
    0.46,
    0.54,
    0.03,
    0.86,
    0.94,
    0.10,
    0.02,
    0.46,
    0.39,
    0.73,
    0.22,
    0.47,
    0.72,
    0.93,
    0.43,
    0.52,
    0.47,
    0.58,
    0.87,
    0.35,
    0.93,
    0.84,
    0.92,
    0.27,
    0.23,
    0.84,
    0.76,
    0.44,
    0.11,
    0.64,
    0.90,
    0.36,
    0.28,
    0.36,
    0.87,
    0.98,
    0.05,
    0.66,
    0.93,
    0.64,
    0.32,
    0.19,
    0.46,
    0.05,
    0.36,
    0.05,
    0.55,
    0.04,
    0.97,
    0.44,
    0.56,
    0.85,
    0.97,
    0.60,
    0.88,
    0.65,
    0.54,
    0.33,
    0.53,
    0.89,
    0.66,
    0.77,
    0.87,
    0.68,
    0.40,
    0.87,
    0.77,
    0.18,
    0.60,
    0.99,
    0.20,
    0.96,
    0.81,
    0.32,
    0.70,
    1.00,
    0.96,
    0.03,
    0.47,
    0.16,
    0.00,
    0.86,
    0.88,
    0.65,
    0.37,
    0.70,
    0.58,
    0.00,
    0.09,
    0.34,
    0.61,
    0.01,
    0.52,
    0.76,
    0.96,
    0.03,
    0.73,
    0.71,
    0.97,
    0.20,
    0.34,
    0.53,
    0.14,
    0.41,
    0.89,
    0.94,
    0.72,
    0.85,
    0.03,
    0.13,
    0.75,
    0.78,
    0.04,
    0.20,
    0.52,
    0.99,
    0.77,
    0.72,
    0.55,
    0.85,
    0.54,
    0.81,
    0.15,
    0.19,
    0.52,
    0.39,
    0.54,
    0.34,
    0.74,
    0.71,
    0.85,
    0.06,
    0.80,
    0.18,
    0.19,
    1.00,
    1.00,
    0.84,
    0.87,
    0.05,
    0.48,
    0.91,
    0.63,
    0.95,
    0.65,
    0.07,
    0.91,
    0.55,
    0.54,
    0.91,
    0.69,
    0.15,
    0.87,
    0.14,
    0.75,
    0.17,
    0.78,
    0.59,
    0.82,
    0.17,
    0.78,
    0.48,
    0.25,
    0.65,
    0.52,
    0.94,
    0.41,
    0.55,
    0.97,
    0.23,
    0.65,
    0.45,
    0.43,
    0.51,
    0.27,
    0.74,
    0.57,
    0.84,
    0.40,
    0.95,
    0.11,
    0.22,
    0.71,
    0.73,
    0.63,
    0.50,
    0.08,
    0.53,
    0.57,
    0.41,
    0.76,
    0.86,
    0.07,
    0.87,
    0.04,
    0.97,
    0.65,
    0.80,
    0.76,
    0.78,
    0.18,
    0.17,
    0.58,
    0.29,
    0.22,
    0.98,
    0.42,
    1.00,
    0.25,
    0.21,
    0.41,
    0.93,
    0.48,
    0.02,
    0.70,
    0.56,
    0.27,
    0.71,
    0.57,
    0.90,
    0.43,
    0.59,
    0.86,
    0.85,
    0.36,
    0.70,
    0.89,
    0.15,
    0.25,
    0.34,
    0.66,
    0.13,
    0.75,
    0.10,
    0.07,
    0.59,
    0.42,
    0.95,
    0.48,
    0.28,
    0.16,
    0.43,
    0.33,
    0.17,
    0.80,
    0.93,
    0.51,
    0.37,
    0.66,
    0.42,
    0.21,
    0.36,
    1.00,
    0.53,
    0.82,
    0.87,
    0.19,
    0.87,
    0.76,
    0.76,
    0.86,
    0.78,
    0.84,
    0.20,
    0.11,
    0.64,
    0.43,
    0.33,
    0.27,
    0.62,
    0.69,
    0.12,
    0.47,
    0.27,
    0.32,
    0.22,
    0.81,
    0.75,
    0.79,
    0.10,
    1.00,
    0.34,
    0.27,
    0.64,
    0.19,
    0.95,
    0.66,
    0.88,
    0.54,
    0.13,
    0.38,
    0.74,
    0.58,
    0.54,
    0.55,
    0.62,
    0.06,
    0.29,
    0.77,
    0.51,
    0.66,
    0.02,
    0.13,
    0.39,
    0.39,
    0.01,
    0.64,
    0.98,
    0.25,
    0.77,
    0.40,
    0.84,
    0.83,
    0.24,
    0.90,
    0.85,
    0.83,
    0.41,
    0.17,
    0.74,
    0.48,
    0.80,
    0.68,
    0.60,
    0.44,
    0.83,
    0.10,
    0.91,
    0.25,
    0.98,
    0.24,
    0.60,
    0.95,
    0.00,
    0.76,
    0.16,
    0.38,
    0.23,
    0.34,
    0.39,
    0.82,
    0.34,
    0.50,
    0.56,
    0.92,
    0.07,
    0.02,
    0.93,
    0.63,
    0.76,
    0.40,
    0.96,
    0.67,
    0.98,
    0.35,
    0.18,
    0.28,
    0.05,
    0.87,
    0.54,
    0.21,
    0.52,
    0.50,
    0.79,
    0.80,
    0.63,
    0.90,
    0.82,
    0.27,
    0.07,
    0.27,
    0.70,
    0.99,
    0.71,
    0.10,
    0.59,
    1.00,
    0.83,
    0.26,
    0.67,
    0.15,
    0.83,
    0.20,
    0.26,
    0.76,
    0.61,
    0.32,
    0.27,
    0.78,
    0.62,
    0.37,
    0.86,
    0.08,
    0.60,
    0.54,
    0.61,
    0.71,
    0.14,
    0.00,
    0.05,
    0.91,
    0.03,
    0.50,
    0.06,
    0.15,
    0.63,
    0.58,
    0.29,
};

#define FIXED_POINT_FRACTIONAL_BITS 16
static int32_t fixed_numbers[NUMBERS_COUNT];

static uint32_t index = 0;

void initialize_random()
{
    for (size_t i = 0; i < NUMBERS_COUNT; i++)
    {
        fixed_numbers[i] = to_fixed16(numbers[i]);
    }
}

void reset_random()
{
    index = 0;
}

float get_random_float()
{
    if (index >= NUMBERS_COUNT)
    {
        index = 0;
    }

    return numbers[index++];
}

int32_t get_random_fixed16()
{
    if (index >= NUMBERS_COUNT)
    {
        index = 0;
    }

    return fixed_numbers[index++];
}
