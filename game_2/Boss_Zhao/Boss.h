/**
 * @file Boss.h
 * @brief Boss object for Boss Battle game
 */

#ifndef BOSS_H
#define BOSS_H

#include <stdint.h>
#include "Utils.h"
#include "Joystick.h"

#include "Bullet.h"
#include "Player.h"

/**
 * @struct Boss_t
 * @brief Boss object containing position, size, and score
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t width;
    int16_t height;
    int16_t speed;
    int16_t direction;
    int16_t hp;
    int16_t max_hp;
    uint32_t last_attack_tick;
    uint32_t attack_interval_ms;

    uint32_t laser_fire_tick;
    uint32_t laser_warning_tick;
    int16_t  laser_vertical;

    int16_t type; // Determine whether it is 1 or 2
} Boss_t;

/**
 * @brief Initialize Boss
 */
void Boss_Init(Boss_t* boss, int16_t x, int16_t y, int16_t type);

/**
 * @brief hit by a bullet
 */
void Boss_TakeDamage(Boss_t* boss);

/**
 * @brief Determine whether the Boss is alive
 */
int Boss_Alive(Boss_t* boss);

/**
 * @brief
 */
void Boss_Update(Boss_t* boss, BulletPool_t* boss_bullets, Player_t* player);

/**
 * @brief Draw Boss on LCD
 */
void Boss_Draw(Boss_t* boss);

/**
 * @brief Get Boss bounding box for collision detection
 */
AABB Boss_GetAABB(Boss_t* boss);

/**
 * @brief Get laser bounding box for collision detection
 */
AABB Boss_GetLaserAABB(Boss_t* boss);

/**
 * @brief Fire bullet
 */
void Boss_Attack_1(Boss_t* boss, BulletPool_t* pool);

/**
 * @brief Drop 5 meteorites
 */
void Boss_Attack_2(Boss_t* boss, BulletPool_t* pool);

/**
 * @brief Fire laser
 */
void Boss_Attack_3(Boss_t* boss);

/**
 * @brief Boss 2 bullets
 */
void Boss_Attack_4(Boss_t* boss, BulletPool_t* pool, Player_t* player);

#endif // BOSS_H
