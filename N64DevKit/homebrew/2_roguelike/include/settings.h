
/**
 * 2D-Roguelike-N64 - settings.h
 */

#ifndef __2D_ROGUELIKE_SETTINGS_H
#define __2D_ROGUELIKE_SETTINGS_H

//COLORS
#define BLACK 0x0
#define WHITE 0xFFFFFFFF

//SCREEN
#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

//MAP Settings
#define MAP_TILES_X 10 //width of map
#define MAP_TILES_Y 8 //height of map
#define MIN_DESTR_WALL 5 //min number of destr walls
#define MAX_DESTR_WALL 9 //maximum number of destr walls
#define MIN_FOOD 1
#define MAX_FOOD 5
#define MAX_ENEMY 10

//GAMESETUP
#define APPLE_HP_RESTORE 10
#define DRINK_HP_RESTORE 100
#define PLAYER_ATTACK_DMG 1
#define PLAYER_START_HP 100
#define PLAYER_ACTION_COST 1 //how much we decrement health by when moving
#define ENEMY_START_HP 2
#define ENEMY_ATTACK_DMG 10
#define ENEMY_ATTACK_DELAY 1500000 //1.5 second delay between enemies actions so they don't all attack at same time
#define MAX_LEVEL 9

//ANIMATION / FRAMERATE
#define FPS 10 //framerate to lock at
#define ANIM_SPEED 6 //animation fps
#define TIMER_ONE_SECOND 1000000

//FONTS
#define PX_PER_CHAR 8

#endif