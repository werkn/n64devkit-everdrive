/**
 * 2D-Roguelike-N64 - player.h
 */

#ifndef __2D_ROGUELIKE_PLAYER_H
#define __2D_ROGUELIKE_PLAYER_H

#include <libdragon.h>
#include "settings.h"
#include "actor.h"
#include "audio.h"
#include "enemy.h"
#include "atlas.h"
#include "game.h"
#include "map.h"

void set_player_state(game_t* game_ref, actor_anim_state_t state);
void draw_player(display_context_t* disp, sprite_t* spritesheet, game_t* game_ref, bool anim_tick);
void move_player(game_t* game_ref, map_t* map_ref, audio_t* audio_ref, u8 dX, u8 dY, void (*stop_rumble_callback)(int ovfl));

#endif