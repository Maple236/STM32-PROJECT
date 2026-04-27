/**
 * @file Bullet.h
 * @brief Bullet object for Boss Battle game
 *
 * Control the Bullet
*/

#ifndef BULLET_H
#define BULLET_H
#define BULLET_POOL_SIZE 32

#include <stdint.h>
#include "Utils.h"
#include "Joystick.h"

/**
 * @struct Bullet_t
 * @brief Bullet object
 */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t size;
    int16_t color;
    Vector2D velocity;
    int16_t active;
} Bullet_t;

/**
 * @struct BulletPool_t
 * @brief Manage all bullets in a pool
 */
typedef struct {
    Bullet_t bullets[BULLET_POOL_SIZE];
} BulletPool_t;

/**
 * @brief Initialize bullet
 */
void Bullet_Init(Bullet_t* bullet);

/**
 * @brief Bullet active or not
 */
int Bullet_Active(Bullet_t* bullet);

/**
 * @brief Set speed and position
 */
void Bullet_Generate(Bullet_t* bullet, int16_t size, int16_t color, int16_t x, int16_t y, float vx, float vy);

/**
 * @brief Set Bullet active = 0
 */
void Bullet_Deactivate(Bullet_t* bullet);

/**
 * @brief Update bullet position based on velocity
 */
void Bullet_Update(Bullet_t* bullet);

/**
 * @brief Draw bullet on LCD
 */
void Bullet_Draw(Bullet_t* bullet);

/**
 * @brief Get bullet bounding box for collision detection
 */
AABB Bullet_GetAABB(Bullet_t* bullet);

/**
 * @brief Get bullet position
 */
Position2D Bullet_GetPos(Bullet_t* bullet);

//================================= Bullet Pool =======================================

/**
 * @brief Initialize all bullets
 */
void BulletPool_Init(BulletPool_t* pool);

/**
 * @brief Generate one bullet from the pool
 */
void BulletPool_Generate(BulletPool_t* pool, int16_t size, int16_t color, int16_t x, int16_t y, float vx, float vy);

/**
 * @brief Update All Active bullets position based on velocity
 */
void BulletPool_UpdateAll(BulletPool_t* pool);

/**
 * @brief Draw All Active bullets on LCD
 */
void BulletPool_DrawAll(BulletPool_t *pool);

#endif // BULLET_H
