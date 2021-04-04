/**
 * 2D-Roguelike-N64 - utils.h
 * 
 * Basic utility methods needed for 2d games.
 */

#ifndef __2D_ROGUELIKE_UTILS_H_
#define __2D_ROGUELIKE_UTILS_H

#include <stdlib.h>
#include <stdbool.h>
#include <libdragon.h>
#include "types.h"
#include "settings.h"

u8 u8_rand_range(u8 min, u8 max);
u8 u8_rand_val(u8* values, u8 length);
u16 text_width(u16 num_of_char, u16 px_per_char);
u16 center_text_x(u16 screen_width, u16 num_of_char, u16 px_per_char);

#endif