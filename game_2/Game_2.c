#include "Game_2.h"
#include "InputHandler.h"
#include "Menu.h"

#include "LCD.h"
#include "Buzzer.h"
#include "Joystick.h"
#include "Bullet.h"
#include "Player.h"
#include "Boss.h"
#include "Utils.h"

#include "stm32l4xx_hal.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern ST7789V2_cfg_t cfg0;
extern Buzzer_cfg_t buzzer_cfg;
extern Joystick_cfg_t joystick_cfg;
extern Joystick_t joystick_data;

MenuState Game2_Run(void) {

    Player_t player;
    Player_Init(&player, 112, 112, 32, 32, 3);

    BulletPool_t player_bullets;
    BulletPool_Init(&player_bullets);

    Boss_t boss;
    Boss_Init(&boss, 150, 80, 1);
    BulletPool_t boss_bullets;
    BulletPool_Init(&boss_bullets);


    // Game display
    int game_over = 0;

    uint32_t last_frame = HAL_GetTick();
    uint32_t last_collisiont_time = 0;

    while (game_over == 0) {
        if (HAL_GetTick() - last_frame < 33) {
            continue;
        }
        last_frame = HAL_GetTick();

        // Read input
        Input_Read();

        Joystick_Read(&joystick_cfg, &joystick_data);
        UserInput input = Joystick_GetInput(&joystick_data);

        if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET) {
            Player_Shoot(&player, &player_bullets);
        }

        LCD_Fill_Buffer(0);

        Player_Update(&player, input);
        Player_Draw(&player);

        BulletPool_UpdateAll(&player_bullets);
        BulletPool_DrawAll(&player_bullets);

        Boss_Update(&boss, &boss_bullets, &player);
        Boss_Draw(&boss);

        BulletPool_UpdateAll(&boss_bullets);
        BulletPool_DrawAll(&boss_bullets);

        // Player bullet hits boss
        AABB boss_box = Boss_GetAABB(&boss);
        for (int i = 0; i < BULLET_POOL_SIZE; i++) {
            if (Bullet_Active(&player_bullets.bullets[i]) != 0) {
                AABB bullet_box = Bullet_GetAABB(&player_bullets.bullets[i]);
                if (AABB_Collides(&bullet_box, &boss_box)) {
                    Boss_TakeDamage(&boss);
                    Bullet_Deactivate(&player_bullets.bullets[i]);
                    Player_AddScore(&player);
                }
            }
        }

        // Boss bullet hits player
        AABB player_box = Player_GetAABB(&player);
        for (int i = 0; i < BULLET_POOL_SIZE; i++) {
            if (Bullet_Active(&boss_bullets.bullets[i]) != 0) {
                AABB bullet_box = Bullet_GetAABB(&boss_bullets.bullets[i]);
                if (AABB_Collides(&bullet_box, &player_box)) {
                    Player_TakeDamage(&player);
                    Bullet_Deactivate(&boss_bullets.bullets[i]);
                }
            }
        }

        // Boss leaser
        if (boss.laser_fire_tick != 0 && HAL_GetTick() - boss.laser_fire_tick < 500) {
            AABB laser_box;
            laser_box = Boss_GetLaserAABB(&boss);
            AABB player_box;
            player_box = Player_GetAABB(&player);
            if (AABB_Collides(&laser_box, &player_box) != 0) {
                Player_TakeDamage(&player);

                // laser hit sound
                buzzer_tone(&buzzer_cfg, 900, 50);
            }
            else {
                // avtive but not hit player
                buzzer_off(&buzzer_cfg);
            }
        }
        else {
                // not active
                buzzer_off(&buzzer_cfg);
        }

        // Boss 2 collision
        if (boss.type == 2) {
            AABB boss_box;
            boss_box = Boss_GetAABB(&boss);

            AABB player_box;
            player_box = Player_GetAABB(&player);
        
            if (AABB_Collides(&boss_box, &player_box) != 0) {
                if (HAL_GetTick() - last_collisiont_time >= 500) {
                    Player_TakeDamage(&player);
                    last_collisiont_time = HAL_GetTick();
                }
            }
        }

        // Game over
        if (Boss_Alive(&boss) <= 0) {
            if (boss.type == 1) {
                LCD_Fill_Buffer(0);
                LCD_printString("NEXT BOSS", 80, 110, 1, 2);
                LCD_Refresh(&cfg0);
                HAL_Delay(2000);
                LCD_Fill_Buffer(0);
                LCD_Refresh(&cfg0);

                Boss_Init(&boss, 200, 100, 2);
                BulletPool_Init(&player_bullets);
                BulletPool_Init(&boss_bullets);
            } 
            else {
                game_over = 1; 
            }
        }
 
        if (Player_Alive(&player) == 0) {
            game_over = 2;
        }

        // Boss HP display
        LCD_Draw_Rect(20, 4, 200, 8, 13, 1);
        LCD_Draw_Rect(20, 4, (200 * boss.hp / boss.max_hp), 8, 2, 1);

        // Player diaplay
        char lives_str[8];
        sprintf(lives_str, "HP:%d", player.lives);
        LCD_printString(lives_str, 4, 4, 1, 1);

        LCD_Refresh(&cfg0);
    }

    if (game_over == 1) {
        buzzer_tone(&buzzer_cfg, 700, 80);  
        HAL_Delay(100);
        buzzer_tone(&buzzer_cfg, 1000, 85); 
        HAL_Delay(100);
        buzzer_tone(&buzzer_cfg, 1400, 90); 
        HAL_Delay(140);
        buzzer_off(&buzzer_cfg);
    } 
    
    if (game_over == 2) {
        buzzer_tone(&buzzer_cfg, 1200, 85); 
        HAL_Delay(120);
        buzzer_tone(&buzzer_cfg, 800, 80);  
        HAL_Delay(140);
        buzzer_tone(&buzzer_cfg, 400, 75);  
        HAL_Delay(180);
        buzzer_off(&buzzer_cfg);
    }

    // Game over & press BT3 to return to menu 
    while (1) {
        LCD_Fill_Buffer(0);
        if (game_over == 1) {
            LCD_printString("YOU WIN", 78, 95, 3, 2);
        } else {
            LCD_printString("GAME OVER", 55, 95, 2, 2);
        }

        char score_str[32];
        sprintf(score_str, "Score: %d", player.score);
        LCD_printString(score_str, 60, 130, 1, 2);

        LCD_printString("Press BT3 to", 60, 175, 1, 1);
        LCD_printString("Return to Menu", 65, 190, 1, 1);

        LCD_Refresh(&cfg0);

        // Read input
        Input_Read();

        // Check if button was pressed to return to menu
        if (current_input.btn3_pressed) {
            break;  // Exit game loop
        }
    }

    return MENU_STATE_HOME;
}
