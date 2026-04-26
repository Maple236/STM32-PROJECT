#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

// Player structure to hold position, velocity, and state
typedef struct {
    int16_t x, y;   //x and y is where is player
    float vy;       //vy is V at vertical (+ = down, - = up)
    int8_t on_ground; //if at ground (1 = yes, 0 = no)
    int8_t facing;  //P is facing where(the bullet need it)
} Player_t;

extern uint8_t player_stand[28][20];
extern uint8_t player_run[28][20];
extern uint8_t player_jump[28][20];

#endif