include "Game_1.h"
#include "InputHandler.h"
#include "Menu.h"
#include "LCD.h"
#include "PWM.h"
#include "Joystick.h" 
#include "Buzzer.h"
#include "stm32l4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/_intsup.h>

//this is which I creat headfile
#include "Player.h"
#include "Enemy.h"
#include "Platform.h"
#include "Bullet.h"
#include "Background.h"

extern ST7789V2_cfg_t cfg0;
extern PWM_cfg_t pwm_cfg;      // LED PWM control
extern Buzzer_cfg_t buzzer_cfg; // Buzzer control
//These two is joystick
extern Joystick_cfg_t joystick_cfg; 
extern Joystick_t joystick_data;


/**
 * @brief Game 1 Implementation - Student can modify
 * 
 * EXAMPLE: Shows how to use PWM LED for visual feedback
 * This is a placeholder with a bouncing animation that changes LED brightness.
 * Replace this with your actual game logic!
 */


#define GRAVITY 0.45f
#define JUMP_VY -7.0f
#define FRAME_MS 30
#define NUM_PLATFORM 10
#define NUM_ENEMY 5
#define NUM_BULLET 6
#define NUM_CLOUD 6
#define NUM_HILL 4


MenuState Game1_Run(void) 
{

    // Initialize player state
    Player_t player = { .x = 20,
                        .y = 200, 
                        .vy = 0, 
                        .on_ground = 0,
                        .facing = 1};
    //camera
    int16_t camera_x = 0;

    //score(when kill one enemy, get 1 score)
    uint16_t score = 0;
    
    //game level (total 5 level, when P pass the last level, it shows YOU WIN)
    int8_t current_level = 0;

    //this is for buzzer
    uint8_t kill_sound_timer = 0;



    //Bullet
    Bullet_t bullets[NUM_BULLET] =
    {
        {.x = 0, .y = 0, .dir = 0, .active = 0},
        {.x = 0, .y = 0, .dir = 0, .active = 0},
        {.x = 0, .y = 0, .dir = 0, .active = 0},
        {.x = 0, .y = 0, .dir = 0, .active = 0},
        {.x = 0, .y = 0, .dir = 0, .active = 0},
        {.x = 0, .y = 0, .dir = 0, .active = 0}
    };

    //different level platform
    Platform_t platforms[NUM_PLATFORM];
    for (int i = 0; i < NUM_PLATFORM; i++)
    {
        platforms[i] = lvl_platforms[current_level][i];
    }

    //different level enemies
    Enemy_t enemies[NUM_ENEMY];
    for (int i = 0; i < NUM_ENEMY; i++)
    {
        enemies[i] = lvl_enemies[current_level][i];
    }

    //flag position
    int16_t flag_x, flag_y;
    flag_x = lvl_flags[current_level][0];
    flag_y = lvl_flags[current_level][1]; 

    while (1) 
    {
        uint32_t t0 = HAL_GetTick();
    
        // Handle input
        Input_Read();
        Joystick_Read(&joystick_cfg, &joystick_data);

        // Check if button was pressed to return to menu
        if (current_input.btn3_pressed) 
        {
            return MENU_STATE_HOME;  // Exit to menu
        }
        player.on_ground = 0;
        player.vy += GRAVITY; //when P is more time at sky, the vy will be bigger
        player.y += (int16_t)player.vy; // use V to represent the position


        // this is detect if P is move.
        if (joystick_data.direction == E) 
        {
            player.x += 3;
            player.facing = 1;
        }
        if (joystick_data.direction == W) 
        {
            player.x += -3;
            player.facing = -1;
        }

        //this is detect if P is over the screen
        if (player.x < 0) 
        {
            player.x = 0;
        }
        if (player.x > 1430) 
        {
            player.x = 1430;
        }

        // this is detect if the P is on the platform
        for(int i = 0; i < NUM_PLATFORM; i++)
        {
            if (player.vy >= 0 && player.x + 20 > platforms[i].x && player.x < platforms[i].x + platforms[i].width && player.y + 28 >= platforms[i].y && player.y < platforms[i].y + 10) 
            {
                player.y = platforms[i].y - 28;
                player.vy = 0;
                player.on_ground = 1;
            }
        }
        
        // this is detect if P is died
        if (player.y > 240) 
        {
            LCD_Fill_Buffer(0);
            LCD_printString("GAME OVER", 40, 110, 1, 3);
            LCD_Refresh(&cfg0);
            while (1) 
            {
                Input_Read();
                if (current_input.btn2_pressed || current_input.btn3_pressed) 
                {
                    break;
                }
                HAL_Delay(20);
            }
            return MENU_STATE_HOME;
        }

        // this is detect if the P jumps, when P jumps, the on_ground will be 0, and the vy is -7.0 which makes the P on the sky
        if(joystick_data.direction == N && player.on_ground == 1)
        {
            player.vy = JUMP_VY;
            player.on_ground = 0;
        }

        // this is detect if bullet shoots
        if (current_input.btn2_pressed) 
        {
            for (int i = 0; i < NUM_BULLET; i++)
            {
                if (bullets[i].active == 0) 
                {
                    bullets[i].x = player.x;
                    bullets[i].y = player.y + 20;
                    bullets[i].dir = player.facing * 5;
                    bullets[i].active = 1;
                    break;
                }

            }
        }

        //this is change the dir of the enemy
        for (int i = 0; i < NUM_ENEMY; i++) 
        {
            if(enemies[i].alive == 1)
            {
                enemies[i].x += enemies[i].dir;
                if (enemies[i].x <= enemies[i].x_min) 
                {
                    enemies[i].dir = 2;
                }
                if (enemies[i].x >= enemies[i].x_max) 
                {
                    enemies[i].dir = -2;
                }
            }
        }

        // this is detect the P and enemy when game over
        for (int i = 0; i < NUM_ENEMY; i++)
        {
            if(enemies[i].alive ==1 && player.x + 20 > enemies[i].x && player.x < enemies[i].x + 10 && player.y + 28 >enemies[i].y && player.y < enemies[i].y + 10)
            {
                LCD_Fill_Buffer(0);
                LCD_printString("GAME OVER", 40, 110, 1, 3);
                LCD_Refresh(&cfg0);                
                while (1) 
                {
                    Input_Read();
                    if (current_input.btn2_pressed || current_input.btn3_pressed) 
                {
                    break;
                }
                    HAL_Delay(20);
                }
                return MENU_STATE_HOME;
            }
        }

        // this is detect the P and flag when display you win
        if (player.x < flag_x + 10 && player.x + 20 > flag_x && player.y < flag_y + 20 && player.y + 28 > flag_y) 
        {
            if (current_level < 4) 
            {
                //this is restart the next level
                current_level++;
                for (int i = 0; i < NUM_PLATFORM; i++)
                {
                    platforms[i] = lvl_platforms[current_level][i];
                }
                for (int i = 0; i < NUM_ENEMY; i++)
                {
                    enemies[i] = lvl_enemies[current_level][i];
                }
                flag_x = lvl_flags[current_level][0];
                flag_y = lvl_flags[current_level][1];

                //player position and the camera_x
                player.x = 20;
                player.y = 200;
                player.vy = 0;
                player.on_ground = 0;
                player.facing = 1;
                camera_x = 0;

                //remind the P go to the next level
                LCD_Fill_Buffer(0);
                LCD_printString("NEXT LEVEL", 40, 110, 1, 3);
                LCD_Refresh(&cfg0);
                HAL_Delay(2000);
            }
            
            else 
            {
                //this is for when pass the level 5
                LCD_Fill_Buffer(0);
                LCD_printString("YOU WIN", 55, 110, 1, 3);
                LCD_Refresh(&cfg0);
                while (1) 
                    {
                        Input_Read();
                        if (current_input.btn2_pressed || current_input.btn3_pressed) 
                    {
                        break;
                    }
                        HAL_Delay(20);
                    }
                return MENU_STATE_HOME;
            }        
        }

        //camera follows P
        camera_x = player.x - 80;
        if(camera_x < 0)
        {
            camera_x = 0;
        }

        //draw the sky
        LCD_Fill_Buffer(11);

        //draw the cloud
        for (int i = 0; i < NUM_CLOUD; i++) 
        {
            int16_t cx = clouds[i][0] - camera_x;
            int16_t cw = 24;
            if (cx + cw > 0  && cx < 240) 
            {
                int16_t draw_x;
                int16_t draw_w;
                if (cx < 0) 
                {
                    draw_x = 0;
                    draw_w = cw + cx;
                }

                else 
                {
                    draw_x = cx;
                    draw_w = cw;
                }
                LCD_Draw_Rect(draw_x, clouds[i][1], draw_w, 6, 1, 1);

                int16_t tx = cx + 4;
                int16_t tw = 14;
                if (tx + tw > 0 && tx < 240) 
                {
                    int16_t tdraw_x;
                    int16_t tdraw_w;
                    if (tx < 0) 
                    {
                        tdraw_x = 0;
                        tdraw_w = tw + tx;
                    }

                    else 
                    {
                        tdraw_x = tx;
                        tdraw_w = tw;
                    }
                    LCD_Draw_Rect(tdraw_x, clouds[i][1]-7, tdraw_w, 9, 1, 1);
                }
            }
        }

        //draw the hills
        for (int i = 0; i < NUM_HILL; i++) 
        {
            int16_t hx = hills[i][0] - camera_x;

            //bottom
            if (hx + 50 > 0 && hx < 240) 
            {
                int16_t draw_x;
                int16_t draw_w;
                if (hx < 0) 
                {
                    draw_x = 0;
                    draw_w = 50 + hx;
                }
                
                else 
                {
                    draw_x = hx;
                    draw_w = 50;
                }
                LCD_Draw_Rect(draw_x, 202, draw_w, 8, 3, 1);
            }

            //middle
            int16_t mx = hx + 8;
            if (mx + 34 > 0 && mx < 240) 
            {
                int16_t draw_x;
                int16_t draw_w;
                if (mx < 0) 
                {
                    draw_x = 0;
                    draw_w = 34 + mx;
                }

                else 
                {
                    draw_x = mx;
                    draw_w = 34;
                }
                LCD_Draw_Rect(draw_x, 194, draw_w, 8, 3, 1);
            }

            //top
            int16_t tx = hx + 16;
            if (tx + 18 > 0 && tx < 240) 
            {
                int16_t draw_x;
                int16_t draw_w;
                if (tx < 0) 
                {
                    draw_x = 0;
                    draw_w = 18 + tx;
                }

                else 
                {
                    draw_x = tx;
                    draw_w = 18;
                }
                LCD_Draw_Rect(draw_x, 186, draw_w, 8, 3, 1);
            }
        }


        
        //Draw Player 
        if (player.on_ground == 0) 
        {
            //jump
            LCD_Draw_Sprite(player.x - camera_x, player.y, 28, 20, (uint8_t*)player_jump);
        }
        else if (joystick_data.direction == E || joystick_data.direction == W) 
        {
            //run
            LCD_Draw_Sprite(player.x - camera_x, player.y, 28, 20, (uint8_t*)player_run);
        }
        else 
        {
            //stop
            LCD_Draw_Sprite(player.x - camera_x, player.y, 28, 20, (uint8_t*)player_stand);
        }
        
        //Draw Platform(which will not be disappear unless the all platform disappear)
        for (int i = 0; i < NUM_PLATFORM; i++) 
        {
            int16_t sx = platforms[i].x - camera_x;
            if (sx + platforms[i].width > 0 && sx < 240) 
            {
                int16_t draw_x;
                int16_t draw_w;
                if (sx < 0) 
                {
                    draw_x = 0;
                    draw_w = platforms[i].width + sx;
                }
                else 
                {
                    draw_x = sx;
                    draw_w = platforms[i].width;
                }
                LCD_Draw_Rect(draw_x, platforms[i].y, draw_w, 6, 5, 1);
            }
        }

        //Draw enemies
        for (int i = 0; i < NUM_ENEMY; i++) 
        {
            if (enemies[i].alive == 1)
            {
                int16_t sx = enemies[i].x - camera_x;
                if (sx >= 0 && sx < 240)
                {
                    LCD_Draw_Sprite(sx, enemies[i].y, 10, 10, (uint8_t*)enemy_sprite);
                }
            }
        }

        //Draw flag
        int16_t sx_flag = flag_x - camera_x;
        LCD_Draw_Rect(sx_flag, flag_y, 10, 20, 6, 1);

        //Bullet moving
        for (int i = 0; i < NUM_BULLET; i++) 
        {
            if (bullets[i].active == 1) 
            {
                bullets[i].x += bullets[i].dir;
                if (bullets[i].x < 0 || bullets[i].x - camera_x > 240)
                {
                    bullets[i].active = 0;
                }
            }
        }

        //bullet kill enemy and adding scores
        for (int i = 0; i < NUM_BULLET; i++)
        {
            if (bullets[i].active == 1) 
            {
                for (int j = 0; j < NUM_ENEMY; j++)
                {
                    if (enemies[j].alive == 1) 
                    {
                        if (bullets[i].x + 2 > enemies[j].x && bullets[i].x < enemies[j].x + 10 && bullets[i].y < enemies[j].y + 10 && bullets[i].y + 5 > enemies[j].y) 
                        {
                            enemies[j].alive = 0;
                            bullets[i].active = 0;
                            score++;
                            buzzer_note(&buzzer_cfg, NOTE_G5, 70);
                            kill_sound_timer = 3;
                            PWM_SetDuty(&pwm_cfg, 0);
                        }
                    }
                }
            }
        }

        //Draw bullet
        for (int i = 0; i < NUM_BULLET; i++)
        {
            if (bullets[i].active == 1) 
            {
                int16_t sx = bullets[i].x -camera_x;
                if (sx >= 0 && sx < 240)
                {    
                    LCD_Draw_Rect(sx, bullets[i].y, 2, 5 , 1, 1);
                }
            }
        }
        
        //draw scores
        char score_buf[16];
        sprintf(score_buf, "Score:%u", score);
        LCD_printString(score_buf, 30, 30, 1, 2);

        //refresh
        LCD_Refresh(&cfg0);

        //this is detect 3 frame then stop
        if (kill_sound_timer > 0) 
        {
            kill_sound_timer = kill_sound_timer - 1;
            if (kill_sound_timer == 0) 
            {
                buzzer_off(&buzzer_cfg);
                PWM_SetDuty(&pwm_cfg, 50);
            }
        }

        //Frame timing
        uint32_t dt = HAL_GetTick() - t0;
        if (dt < FRAME_MS) 
        {
            HAL_Delay(FRAME_MS - dt);
        }
        
    }
}
