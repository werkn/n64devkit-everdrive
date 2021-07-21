/**
 * 2D-Roguelike-N64 - atlas.h
 */

#ifndef __2D_ROGUELIKE_ATLAS_H
#define __2D_ROGUELIKE_ATLAS_H

//sprite indexes based on .sprite file produced from tilesheet
//found in /res/spritesheet.png
enum sprite_atlas {
    PLAYER_IDLE_START = 0,
    PLAYER_IDLE_END = 5,
    PLAYER_HIT_START = 46,
    PLAYER_HIT_END = 47,
    PLAYER_ATTACK_START = 40,
    PLAYER_ATTACK_END = 41,
    ENEMY_A_IDLE_START = 6,
    ENEMY_A_IDLE_END = 11,
    ENEMY_A_HIT_START = 43,
    ENEMY_A_HIT_END = 43,
    ENEMY_A_ATTACK_START = 42,
    ENEMY_A_ATTACK_END = 43,
    ENEMY_B_IDLE_START = 12,
    ENEMY_B_IDLE_END = 17,
    ENEMY_B_HIT_START = 44,
    ENEMY_B_HIT_END = 44,
    ENEMY_B_ATTACK_START = 44,
    ENEMY_B_ATTACK_END = 45,
    DRINK = 18,
    APPLES = 19,
    EXIT = 20,
    EXT_WALL_A = 26,
    EXT_WALL_B = 28,
    DESTR_WALL_A = 30,
    DESTR_WALL_A_DMG = 53,
    DESTR_WALL_B = 31,
    DESTR_WALL_B_DMG = 54,
    GND_TILE_START = 33,
    GND_TILE_END = 39,
    BLANK_TILE = 56
};

#endif