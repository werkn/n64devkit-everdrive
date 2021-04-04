
/**
 * 2D-Roguelike-N64 - tiles.h
 */

#ifndef __2D_ROGUELIKE_TILES_H
#define __2D_ROGUELIKE_TILES_H

#include "types.h"

typedef enum tile_tags {
    TAG_NONE, //make TAG_NONE first element so we default fill everything with TAG_NONE
    TAG_ENEMY_A,
    TAG_ENEMY_B,
    TAG_APPLES,
    TAG_DRINK,
    TAG_WALL,
    TAG_OUTER_WALL,
    TAG_PLAYER,
    TAG_EXIT
} tile_tags_t;

//stores a tile id
typedef struct {
    //currently on stores one tile id, but if we want multiple layers of tiles
    //we would just need to add more tile_id and we could have multi-layer tiles
    u8 tile_id;
    tile_tags_t tag;
} tile_t;

#endif