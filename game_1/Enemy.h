#ifndef ENEMY_H
#define ENEMY_H

#include <stdint.h>

typedef struct {
    int16_t x, y;
    int8_t dir;
    int16_t x_min, x_max;
    int8_t alive;
} Enemy_t;

extern uint8_t enemy_sprite[10][10];
extern const Enemy_t lvl_enemies[5][5];

#endif