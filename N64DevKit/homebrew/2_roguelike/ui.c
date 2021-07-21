#include "include/ui.h"

void draw_ui(display_context_t* disp, sprite_t* spritesheet, game_t* game_ref) 
{
    //temp string for storing game text, shouldn't need more then 32 characters
    char tmp_str[32];

    //set the text output color to white initially
    //format here is graphics_set_color(text_background_fill, text_foreground_fill)
    graphics_set_color(WHITE, BLACK);
    
    u16 len = sprintf(tmp_str, "Food:  %d", game_ref->player.hp); 
    graphics_draw_text( *disp, center_text_x(SCREEN_WIDTH, len, PX_PER_CHAR), 210, tmp_str);
    //alert user when no input devices (ie: controllers are found indicated by -1)
    if (game_ref->controller_id == -1) 
    {
        graphics_set_color(WHITE, BLACK);
        graphics_draw_text(*disp, 25, 120, "No controllers connected!");
    } 

    if (identify_accessory(game_ref->controller_id) == ACCESSORY_RUMBLEPAK) { graphics_draw_text(*disp, 220, 10, "RUMBLE ON"); }
    else { graphics_draw_text(*disp, 220, 10, "RUMBLE OFF"); }
}

void draw_title_screen(display_context_t* disp, game_t* game_ref) 
{
    char tmp_str[12];

    //set the text output color to white initially
    graphics_set_color(WHITE, BLACK);
    u16 len = sprintf(tmp_str, "Day %d\n", game_ref->current_level);
    graphics_draw_text( *disp, center_text_x(SCREEN_WIDTH, len, PX_PER_CHAR), 120, tmp_str);
}

void draw_game_over_screen(display_context_t* disp, game_t* game_ref) 
{
    char tmp_str[40];

    //set the text output color to white initially
    graphics_set_color(WHITE, BLACK);
    
    u16 len = sprintf(tmp_str, "After %d days, you starved.\n", game_ref->current_level); 
    graphics_draw_text( *disp, center_text_x(SCREEN_WIDTH, len, PX_PER_CHAR), 120, tmp_str);
    
    len = sprintf(tmp_str, "Press <Z> to restart!\n"); 
    graphics_draw_text( *disp, center_text_x(SCREEN_WIDTH, len, PX_PER_CHAR), 140, tmp_str);
}