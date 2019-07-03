#ifndef SKY_MATH_UTILS
#define SKY_MATH_UTILS

#include <math.h>

// Floors a float to the nearest int value below it. This onloy works for values in the range of SHRT_MIN to SHRT_MAX.
static inline int fast_floor(float x) {
    return (int)(x + 32768.0f) - 32768;
}

static inline float floor_ext(float value, float n) {
    return floorf(value / n) * n;
}

#endif