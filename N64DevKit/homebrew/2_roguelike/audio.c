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

#include "include/audio.h"

audio_t *audio_setup(const u16 sample_rate, const u8 buffers)
{
    /* Start up the audio subsystem */
    audio_init( sample_rate, buffers );  //note: more audio buffers means less audio glitch but increased latency
    const u16 buffer_length = audio_get_buffer_length();
    s16 *buffer = malloc( buffer_length * STEREO_PCM_SAMPLE_SIZE );
    audio_t *audio = malloc( sizeof( audio_t ) );
    audio->sample_rate = sample_rate;
    audio->frames = buffer_length << 1;
    audio->buffer = buffer;

    /* Load the sound effects cache */
    char *sfx_files[SFX_NUM_SOUNDS] = {
        "/sfx/scavengers_chop1.raw",
        "/sfx/scavengers_fruit1.raw",
        "/sfx/scavengers_soda1.raw",
        "/sfx/scavengers_enemy1.raw",
        "/sfx/scavengers_footstep1.raw",
        "/sfx/scavengers_footstep2.raw",
        "/sfx/scavengers_music.raw",
        "/sfx/scavengers_hit.raw"
    };

    pcm_sound_t *sfx;
    for (u8 i = 0; i < SFX_NUM_SOUNDS; i++)
    {
        sfx = read_dfs_pcm_sound( sfx_files[i], sample_rate, 1 );
        audio->sfx_cache[i] = sfx;
    }
    /* Setup the sound effects channels */
    for (u8 i = 0; i < SFX_NUM_CHANNELS; i++)
    {
        audio->channels[i].cursor = 0;
        audio->channels[i].sfx = NULL;
    }
    return audio;
}

void audio_free(audio_t *audio)
{
    /* Clear the sound effects cache */
    for (u8 i = 0; i < SFX_NUM_SOUNDS; i++)
    {
        free( audio->sfx_cache[i]->data );
        free( audio->sfx_cache[i] );
        audio->sfx_cache[i] = NULL;
    }
    /* Clear sound effects pointers from playback channels */
    for (u8 i = 0; i < SFX_NUM_CHANNELS; i++)
    {
        audio->channels[i].sfx = NULL;
    }
    /* Shut down the audio subsystem */
    free( audio->buffer );
    free( audio );
    audio_close();
}

inline static s16 mix_pcm_samples(s32 mix, u8 num)
{
    return (num > 1) ? mix / num : mix;
}

void audio_tick(audio_t *audio)
{
    if ( audio != NULL && audio_can_write() )
    {
        sfx_channel_t channel;
        pcm_sound_t *sfx;
        s32 left_mix, right_mix;
        u8 left_num, right_num;

        /* Fill the audio buffer with stereo sample frames */
        for (u16 frame = 0; frame < audio->frames;
             left_mix = left_num = right_mix = right_num = 0)
        {
            /* Accumulate all currently playing sound effects samples */
            for (u8 i = 0; i < SFX_NUM_CHANNELS; i++)
            {
                channel = audio->channels[i];
                sfx = channel.sfx;
                if ( sfx != NULL && channel.cursor < sfx->samples )
                {
                    left_mix += sfx->data[channel.cursor++];
                    left_num++;
                    /* Play mono sound effects in both speakers */
                    if ( sfx->channels == 1 )
                    {
                        right_mix += sfx->data[channel.cursor - 1];
                        right_num++;
                    }
                    /* Play stereo sound effects in separate speakers */
                    else if ( channel.cursor < sfx->samples )
                    {
                        right_mix += sfx->data[channel.cursor++];
                        right_num++;
                    }
                    /* Reset channels that have finished playing */
                    if ( channel.cursor >= sfx->samples )
                    {
                        channel.cursor = 0;
                        channel.sfx = NULL;
                    }
                }
                audio->channels[i] = channel;
            }
            /* Mix down all of the samples as an average */
            audio->buffer[frame++] = mix_pcm_samples( left_mix, left_num );
            audio->buffer[frame++] = mix_pcm_samples( right_mix, right_num );
        }
        audio_write( audio->buffer );
    }
}

void audio_play_sfx(audio_t *audio, const sfx_sounds_t sfx_sound)
{
    if ( audio != NULL )
    {
        pcm_sound_t *sfx = audio->sfx_cache[sfx_sound];
        if ( sfx != NULL )
        {
            for (u8 i = 0; i < SFX_NUM_CHANNELS; i++)
            {
                if ( audio->channels[i].sfx == NULL )
                {
                    audio->channels[i].sfx = sfx;
                    //tag sound effect id in channel
                    audio->channels[i].sfx->sfx_id = sfx_sound;
                    audio->channels[i].cursor = 0;
                    break;
                }
            }
        }
    }
}

pcm_sound_t *read_dfs_pcm_sound(char *file, u16 sample_rate, u8 channels)
{
    int fp = dfs_open( file );
    const u32 size = dfs_size( fp );
    s16 *data = malloc( size );
    dfs_read( data, 1, size, fp );
    dfs_close( fp );
    u32 frames = size / MONO_PCM_SAMPLE_SIZE;
    pcm_sound_t *sound = malloc( sizeof( pcm_sound_t ) );
    sound->sample_rate = sample_rate;
    sound->channels = channels;
    sound->frames = frames;
    sound->samples = frames / channels;
    sound->data = data;
    return sound;
}

//checks whether the provided sound is playing in any sound channel
bool is_playing_sfx(audio_t *audio, sfx_sounds_t sfx_sound)
{
    for (u8 i = 0; i < SFX_NUM_CHANNELS; i++)
    {
        //check if sfx_sound is being played (if it is it will be in audio->channels[i].sfx])
        if ( audio->channels[i].sfx != NULL && audio->channels[i].sfx->sfx_id == sfx_sound)
        {
            return true;
        }
    }
    return false;
}

//returns an array of sfx_sound_t = SFX_NUM_CHANNELS
//that lists whats playing (if anything) in that channel
sfx_sounds_t get_sfx_in_channel(audio_t *audio, int channel) 
{
        if ( audio->channels[channel].sfx != NULL ) { return audio->channels[channel].sfx->sfx_id; }
        else { return SFX_NOT_PLAYING; }

}

