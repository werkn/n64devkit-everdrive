#include "include/debug.h"

static volatile uint8_t fps;
static volatile uint8_t ops;
//fps = frams per second achieved
static uint8_t fps_ticks;
//ops = main loops per second achieved
static uint8_t ops_ticks;

void draw_debug(display_context_t* disp, audio_t* audio_ref) 
{
    char tmp_str[24];

    //set the text output color to white initially
    graphics_set_color(WHITE, BLACK);
    u16 len = sprintf(tmp_str, "FPS %d OPS %d\n", fps, ops);
    graphics_draw_text( *disp, center_text_x(SCREEN_WIDTH, len, PX_PER_CHAR), 15, tmp_str);

    if (audio_ref->draw_debug)
    {
        draw_audio_channels(disp, audio_ref);
    }
}

//displays on screen which channels are playing what sounds
void draw_audio_channels(display_context_t* disp, audio_t* audio_ref)
{
    //draw what is playing in each audio channel
    graphics_set_color(WHITE, BLACK);

    for (u8 i = 0; i < SFX_NUM_CHANNELS; i++) {
        char sfx_id[32]; 
        sprintf(sfx_id, "AUDIO_CH_%d -> %d \n", i, get_sfx_in_channel(audio_ref, i));
        graphics_draw_text( *disp, 10, 40 + (i*10), sfx_id);
    }

    if (is_playing_sfx(audio_ref, SFX_MUSIC)) 
    {
        graphics_draw_text( *disp, 10, 30, "Music (ON)");
    } else {
        graphics_draw_text( *disp, 10, 30, "Music (OFF)");
    }
}

void fps_tick()
{
    fps_ticks++;
}

void ops_tick()
{
    ops_ticks++;
}

//called externally and reset via new_timer()
void reset_debug_ticks()
{
    fps = fps_ticks;
    fps_ticks= 0;
    
    ops = ops_ticks;
    ops_ticks= 0;
}