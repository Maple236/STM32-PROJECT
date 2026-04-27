/**
 * @file Bullet.c
 * @brief Bullet object implementation for Boss Battle game
 */

 #include "Bullet.h"
 #include "LCD.h"
#include <stdint.h>

// Screen dimensions (ST7789V2 display)
#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 240

void Bullet_Init(Bullet_t* bullet) {
    bullet->active = 0;
}

int Bullet_Active(Bullet_t* bullet) {
    return bullet->active;
}

void Bullet_Generate(Bullet_t* bullet, int16_t size, int16_t color, int16_t x, int16_t y, float vx, float vy) {
    bullet->size = size;
    bullet->color = color;
    bullet->x = x;
    bullet->y = y;
    bullet->velocity.x = vx;
    bullet->velocity.y = vy;
    bullet->active = 1;
}

void Bullet_Deactivate(Bullet_t* bullet) {
    bullet->active = 0;
}

void Bullet_Update(Bullet_t* bullet) {
    bullet->x += (int16_t)bullet->velocity.x;
    bullet->y += (int16_t)bullet->velocity.y;
}


void Bullet_Draw(Bullet_t* bullet) {
    LCD_Draw_Circle(
        bullet->x + bullet->size / 2,
        bullet->y + bullet->size / 2,
        bullet->size / 2,
        bullet->color, 
        1
    );
}

AABB Bullet_GetAABB(Bullet_t* bullet) {
    AABB box;
    box.x = bullet->x;
    box.y = bullet->y;
    box.width = bullet->size;
    box.height = bullet->size;
    return box;
}

Position2D Bullet_GetPos(Bullet_t* bullet) {
    Position2D pos;
    pos.x = bullet->x;
    pos.y = bullet->y;
    return pos;
}

//================================= Bullet Pool =======================================
void BulletPool_Init(BulletPool_t* pool) {
    for (int i = 0; i < BULLET_POOL_SIZE; i++) {
        Bullet_Init(&pool->bullets[i]);
    }
}

void BulletPool_Generate(BulletPool_t* pool, int16_t size, int16_t color, int16_t x, int16_t y, float vx, float vy) {
    for (int i = 0; i < BULLET_POOL_SIZE; i++) {
        if (Bullet_Active(&pool->bullets[i]) != 1) {
            Bullet_Generate(&pool->bullets[i], size, color, x, y, vx, vy);
            return;
        }
    }
}

void BulletPool_UpdateAll(BulletPool_t* pool) {
    for (int i = 0; i < BULLET_POOL_SIZE; i++) {
        if (Bullet_Active(&pool->bullets[i]) != 0) {
            Bullet_Update(&pool->bullets[i]);
            if (pool->bullets[i].x < 0 || pool->bullets[i].x > SCREEN_WIDTH || pool->bullets[i].y < 0 || pool->bullets[i].y > SCREEN_HEIGHT) {
                Bullet_Deactivate(&pool->bullets[i]);
            }
        }
    }
}

void BulletPool_DrawAll(BulletPool_t *pool) {
    for (int i = 0; i < BULLET_POOL_SIZE; i++) {
        if (Bullet_Active(&pool->bullets[i]) != 0) {
            Bullet_Draw(&pool->bullets[i]);
        }
    }
}
