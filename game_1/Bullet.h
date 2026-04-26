#ifndef BULLET_H
#define BULLET_H

//Bullet
typedef struct {
    int16_t x, y;
    int8_t dir; // 1 is right, -1 is left
    int8_t active; // 1 is flying, 0 is not
} Bullet_t;

#endif