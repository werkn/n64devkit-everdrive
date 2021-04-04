/**
 * 2D-Roguelike-N64 - enemy.h
 */

#ifndef __WERKNLIB_FPS_H
#define __WERKNLIB_FPS_H

#include <libdragon.h>
#include <stdio.h>
#include <stdlib.h>
#include "audio.h"
#include "settings.h"
#include "types.h"
#include "utils.h"

void draw_debug(display_context_t* disp, audio_t* audio_ref);
void draw_audio_channels(display_context_t* disp, audio_t* audio_ref);
void ops_tick();
void fps_tick();
void reset_debug_ticks();

#endif