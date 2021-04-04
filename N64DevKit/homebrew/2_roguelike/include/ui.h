/**
 * 2D-Roguelike-N64 - ui.h
 */

#ifndef __2D_ROGUELIKE_UI_H
#define __2D_ROGUELIKE_UI_H

#include <libdragon.h>
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "utils.h"
#include "settings.h"
#include "input.h"

void draw_ui(display_context_t* disp, sprite_t* spritesheet, game_t* game_ref);
void draw_title_screen(display_context_t* disp, game_t* game_ref);
void draw_game_over_screen(display_context_t* disp, game_t* game_ref);

#endif