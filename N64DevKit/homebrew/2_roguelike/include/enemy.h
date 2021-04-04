/**
 * 2D-Roguelike-N64 - enemy.h
 */

#ifndef __2D_ROGUELIKE_ENEMY_H
#define __2D_ROGUELIKE_ENEMY_H

#include <libdragon.h>
#include "actor.h"
#include "player.h"
#include "atlas.h"
#include "map.h"

void set_enemy_state(map_t* map, actor_anim_state_t state, u8 enemy_index); 
void draw_enemies(display_context_t* disp, sprite_t* spritesheet, map_t* map_ref, bool anim_tick);
void move_enemy(game_t* game_ref, map_t* map_ref, audio_t* audio_ref, u8 enemy_index, void (*stop_rumble_callback)(int ovfl));
void move_enemies(game_t* game_ref, map_t* map_ref, audio_t* audio_ref, void (*stop_rumble_callback)(int ovfl));

#endif