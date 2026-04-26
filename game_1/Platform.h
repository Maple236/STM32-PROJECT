#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

typedef struct {
    int16_t x, y;
    int16_t width;
} Platform_t;

extern const Platform_t lvl_platforms[5][10];
extern const int16_t lvl_flags[5][2];

#endif