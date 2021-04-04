/**
 * Modified version of the audio.c/audio.h found in the FlappyBird N64 repo 
 *
 * Copyright 2017, Christopher Bonhage
 *
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * Modified (slightly) Ryan Radford.
 * 
 * >>>Changes
 * Added the ability to tag sounds that are currenlty playing in each channel.
 * 
 * Methods:
 * bool is_playing_sfx(audio_t *audio, sfx_sounds_t sfx_sound)
 * ... checks whether a specific sound is playing in any audio channel
 * 
 * sfx_sounds_t get_sfx_in_channel(audio_t *audio, int channel) 
 * ... returns SFX currenlty playing in channel
 * -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 * 
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */


#ifndef __2D_ROGUELIKE_AUDIO_H
#define __2D_ROGUELIKE_AUDIO_H

#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <libdragon.h>

#include "types.h"

/* Audio definitions */
#define FREQUENCY_44KHZ 44100
#define MONO_PCM_SAMPLE_SIZE sizeof(s16)
#define STEREO_PCM_SAMPLE_SIZE (MONO_PCM_SAMPLE_SIZE << 1)

/* Sound FX definitions */
#define SFX_NUM_CHANNELS    8
#define SFX_NUM_SOUNDS      9

typedef enum sfx_sounds
{
    SFX_PLAYER_CHOP_1,
    SFX_APPLE,
    SFX_SODA,
    SFX_ENEMY_DIE,
    SFX_FOOTSTEP_1,
    SFX_FOOTSTEP_2,
    SFX_MUSIC,
    SFX_HIT,
    SFX_NOT_PLAYING //indicates channel is not playing audio, MUST ALWAYS BE LAST in enum
} sfx_sounds_t;

typedef struct
{
    u16 sample_rate;
    u8 channels;
    u32 frames;
    u32 samples;
    s16 *data;
    sfx_sounds_t sfx_id;
} pcm_sound_t;

typedef struct
{
    u32 cursor;
    pcm_sound_t *sfx;
} sfx_channel_t;

typedef struct
{
    // Setup state
    u16 sample_rate;
    u32 frames;
    s16 *buffer;
    pcm_sound_t *sfx_cache[SFX_NUM_SOUNDS];
    // Playback state
    sfx_channel_t channels[SFX_NUM_CHANNELS];
    bool draw_debug;
} audio_t;

/* Audio functions */

audio_t *audio_setup(const u16 sample_rate, const u8 buffers);

void audio_free(audio_t *audio);

void audio_tick(audio_t *audio);

void audio_play_sfx(audio_t *audio, const sfx_sounds_t sfx_sound);

//checks audio_t->channels for a specific sound sfx, if its there its playing if its not
//its not playing
bool is_playing_sfx(audio_t *audio, sfx_sounds_t sfx_sound);

//get the sfx as sfx_sound_t currenlty playing in channel
sfx_sounds_t get_sfx_in_channel(audio_t *audio, int channel) ;

char* get_sfx_filename(audio_t *audio, sfx_sounds_t sfx_sound);

pcm_sound_t *read_dfs_pcm_sound(char *file, u16 sample_rate, u8 channels);

#endif