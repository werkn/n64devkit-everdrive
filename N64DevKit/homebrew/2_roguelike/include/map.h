/**
 * 2D-Roguelike-N64 - map.h
 */

#ifndef __2D_ROGUELIKE_MAP_H
#define __2D_ROGUELIKE_MAP_H

#include <libdragon.h>
#include <stdbool.h>
#include <math.h>
#include "tiles.h"
#include "actor.h"
#include "atlas.h"
#include "game.h"
#include "utils.h"
#include "settings.h"
#include "types.h"

typedef struct {
    tile_t tiles[MAP_TILES_X][MAP_TILES_Y];
    u8 active_enemy_id;
    u8 num_of_enemy;
    actor_t enemies[MAX_ENEMY];
    bool enemy_doing_action;
    u8 exit_spawn_x;
    u8 exit_spawn_y;
    bool draw_debug;
} map_t;


void spawn_random_location(map_t *map, u8 num_to_spawn, u8* tile_list, u8 num_tiles_in_list, bool spawn_enemy);
void generate_map(map_t* map_ref, game_t* game_ref);
void draw_map(display_context_t* disp, sprite_t* spritesheet, map_t* map_ref, bool draw_tags);
void wall_hit(map_t* map_ref, u8 x, u8 y);
tile_tags_t get_tag_at_x_y(map_t* map_ref, u8 x, u8 y);

#endif