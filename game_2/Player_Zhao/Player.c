/**
 * @file Player.c
 * @brief Player object implementation for Boss Battle game
 */

#include "Player.h"
#include "LCD.h"
#include "stm32l4xx_hal.h"
#include <stdint.h>

// Screen dimensions (ST7789V2 display)
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH  240

static const uint8_t player_sprite[16][16] = {
    {255,255,255,255,255,  8,255,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255,  8,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,  8,  8,  8,255,255,255,255,255,255,255,255},
    {255,255,255,255,  8,  8,  8,  8,  8,255,255,255,255,255,255,255},
    {255,255,255,255,255, 11, 11,  6,255,255,255,255,255,255,255,255},
    {255,255,255,255,255, 11, 11, 11,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,255, 11,255,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,  8,  8,  8,  8,  8,  9,255,255,255,255,255},
    {255,255,255,255,255,  8,  8,  8,255,255,255,255,255,255,255,255},
    {255,255,255,255,255,  8,  8,  8,255,255,255,255,255,255,255,255},
    { 14,255,255,255,255,  8,  8,  8,255,255,255,255,255,255,255,255},
    {255, 14,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,255,255,255},
    { 14, 14,255,255,255,255,  8,255,  8,255,255,255,255,255,255,255},
    { 14,255,255,255,255,255,255,  8,255,  8,255,255,255,255,255,255},
    {255,255,255,255,255,255,  8,255,255,  8,255,255,255,255,255,255},
    {255,255,255,255,  0,  8,255,255,  0,255,255,255,255,255,255,255},
};

void Player_Init(Player_t* player, int16_t x, int16_t y, int16_t width, int16_t height, int16_t speed) {
    player->x = x;
    player->y = y;
    player->width = width;
    player->height = height;
    player->speed = speed;
    player->score = 0;
    player->lives = 5;
    player->last_shoot_tick = 0;
    player->shoot_coolingtime_ms = 300;
}

void Player_TakeDamage(Player_t* player) {
    if (player->lives > 0) {
        player->lives = player->lives - 1;
    }
}

int Player_Alive(Player_t* player) {
    return player->lives;
}

void Player_Shoot(Player_t* player, BulletPool_t* pool) {
    uint32_t now = HAL_GetTick();
    if (now - player->last_shoot_tick < player->shoot_coolingtime_ms) {
        return;
    }
    BulletPool_Generate(
        pool,
        4,
        1,
        player->x + player->width,
        player->y + player->height/2,
        15.0f,
        0.0f);
    player->last_shoot_tick = now;
}

void Player_Update(Player_t* player, UserInput input) {
    int16_t dx = 0;
    int16_t dy = 0;

    switch (input.direction) {
        case N:  dy = -player->speed; break;
        case NE: dy = -player->speed; dx =  player->speed; break;
        case E:  dx =  player->speed; break;
        case SE: dy =  player->speed; dx =  player->speed; break;
        case S:  dy =  player->speed; break;
        case SW: dy =  player->speed; dx = -player->speed; break;
        case W:  dx = -player->speed; break;
        case NW: dy = -player->speed; dx = -player->speed; break;
        default: break;
    }

    if (dx != 0 || dy != 0) {
        player->x = (int16_t)player->x + dx;
        player->y = (int16_t)player->y + dy;

        if (player->x < 0) player->x = 0;
        if (player->x >= (SCREEN_WIDTH - player->width)) player->x = SCREEN_WIDTH - player->width;
        if (player->y < 0) player->y = 0;
        if (player->y >= (SCREEN_HEIGHT - player->height)) player->y = SCREEN_HEIGHT - player->height;
    }
}

void Player_Draw(Player_t* player) {
    LCD_Draw_Sprite_Scaled(player->x, player->y, 16, 16, (const uint8_t*)player_sprite, 2);
}

AABB Player_GetAABB(Player_t* player) {
    AABB box;
    box.x = player->x;
    box.y = player->y;
    box.width = player->width;
    box.height = player->height;
    return box;
}

void Player_AddScore(Player_t* player) {
    player->score++;
}

Position2D Player_GetPos(Player_t* player) {
    Position2D pos;
    pos.x = player->x;
    pos.y = player->y;
    return pos;
}
