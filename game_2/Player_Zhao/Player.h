/**
 * @file Player.h
 * @brief Player object for Boss Battle game
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>
#include "Utils.h"
#include "Joystick.h"

#include "Bullet.h"

/**
 * @struct Player_t
 * @brief Player object containing position, size, and score
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    int16_t speed;
    uint16_t score;
    int16_t lives;
    uint32_t last_shoot_tick;
    uint32_t shoot_coolingtime_ms;
} Player_t;

/**
 * @brief Initialize player
 */
void Player_Init(Player_t* player, int16_t x, int16_t y, int16_t width, int16_t height, int16_t speed);

/**
 * @brief hit by a bullet
 */
void Player_TakeDamage(Player_t* player);

/**
 * @brief Determine whether the player is alive
 */
int Player_Alive(Player_t* player);

/**
 * @brief
 */
void Player_Shoot(Player_t* player, BulletPool_t* pool);

/**
 * @brief Update player position based on joystick input
 */
void Player_Update(Player_t* player, UserInput input);

/**
 * @brief Draw Player on LCD
 */
void Player_Draw(Player_t* player);

/**
 * @brief Get player bounding box for collision detection
 */
AABB Player_GetAABB(Player_t* player);

/**
 * @brief Increment player score
 */
void Player_AddScore(Player_t* player);

/**
 * @brief Get player position
 */
Position2D Player_GetPos(Player_t* player);

#endif // PLAYER_H
