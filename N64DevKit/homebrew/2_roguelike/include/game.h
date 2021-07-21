/**
 * 2D-Roguelike-N64 - game.h
 */

#ifndef __2D_ROGUELIKE_GAME_H
#define __2D_ROGUELIKE_GAME_H

#include <stdbool.h>
#include "types.h"
#include "actor.h"

typedef enum scene {
    SCENE_DAY_TRANSITION,
    SCENE_IN_GAME,
    SCENE_GAME_OVER
} scene_t;

typedef struct game {
    actor_t player;
    scene_t current_scene;
    u8 player_spawn_x;
    u8 player_spawn_y;
    u8 controller_id;
    u8 current_level; //max level is 255
    bool player_turn;
    bool player_doing_action; //used to stop player spamming attack on walls, enemies
    
    //if exit on next frame, then player has touched exit, advance level 
    //on next loop
    bool exit_on_next_frame;
    //if game over on next frame, display game over screen
    bool game_over_on_next_frame;
    //if reset on next frame, reset entire game
    bool reset_on_next_frame;
    bool show_debug;
    bool music_playing;
} game_t;

#endif
