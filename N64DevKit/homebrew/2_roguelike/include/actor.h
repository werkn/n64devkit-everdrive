/**
 * 2D-Roguelike-N64 - actor.h
 */

#ifndef __2D_ROGUELIKE_ACTOR_H
#define __2D_ROGUELIKE_ACTOR_H

#include "types.h"
#include "tiles.h"

typedef enum actor_anim_state {
    ACTOR_ATTACKING,
    ACTOR_HIT,
    ACTOR_IDLE,
    ACTOR_DEAD
} actor_anim_state_t;

typedef struct {
    u8 x; //position is a tile offset, not px so we can use a u8 here
    u8 y;
    u16 hp; 
    u8 tile_id; //active animation frame
    tile_tags_t tag; //special tag to identify what actor is when in an array
    actor_anim_state_t state; 
} actor_t;

#endif