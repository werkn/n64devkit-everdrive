/**
 * 2D-Roguelike-N64 - map.h
 */

#ifndef __2D_ROGUELIKE_INPUT_H
#define __2D_ROGUELIKE_INPUT_H

#include <libdragon.h>
#include "game.h"
#include "tiles.h"
#include "audio.h"
#include "settings.h"
#include "player.h"

void handle_input(game_t* game_ref, map_t* map_ref, audio_t* audio_ref, void (*callback)(int ovfl));

#endif