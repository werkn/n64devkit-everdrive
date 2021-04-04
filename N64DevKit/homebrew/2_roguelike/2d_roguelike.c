/**
 * Attempt at figuring out what Libdragon is capable of.  
 * This project is a recreation of the Unity 2D Roguelike sample 
 * found at: https://youtu.be/Fdcnt2-Jf4w made in Unity.
 * 
 * It's basic.
 * It has bugs.
 * But most importantly it gave me a good sense of what Libdragon is capable of (currently).
 * 
 * Of all the things learned, Audio, is probably going to be the biggest headache moving forward.
 */

#include "include/utils.h"
#include "include/settings.h"
#include "include/atlas.h"
#include "include/actor.h"
#include "include/tiles.h"
#include "include/map.h"
#include "include/game.h"
#include "include/ui.h"
#include "include/input.h"
#include "include/debug.h"
#include "include/player.h"
#include "include/enemy.h"
#include "include/audio.h"

static map_t map;
static game_t game;

static audio_t *audio = NULL;

//callback functions used with timers
static bool redraw_tick; //whether to swap buffers
void redraw() { redraw_tick = true; }

void set_scene_in_game() { 
    game.current_scene = SCENE_IN_GAME; 

    //restart game audio if not muted
    if (!is_playing_sfx(audio, SFX_MUSIC)) {
        audio_play_sfx(audio, SFX_MUSIC);
    }
}

static bool anim_tick; //whether to advance animation frames globally
void update_anim() { anim_tick = true; } //flag set here and reset once animations are advanced in draw methods

static bool buffer_audio; //wether to do audio tick
void update_audio() { buffer_audio = true; }

//no arg callback to be used with timer to stop rumbling after 1 second
//not placed in input.c because I don't know how to do callbacks with arguments yet :(
//but it should probably live there
void stop_rumble() { rumble_stop(game.controller_id); }

void setup() 
{
    game.player_spawn_x = 1;
    game.player_spawn_y = MAP_TILES_Y - 2;
    game.player.x = game.player_spawn_x;
    game.player.y = game.player_spawn_y;
    game.player_turn = true;
    game.player_doing_action = false;
    set_player_state(&game, ACTOR_IDLE);

    map.active_enemy_id = 0;
    map.enemy_doing_action = false;
    map.num_of_enemy = 0;
    for (u8 i = 0; i < MAX_ENEMY; i++) {
        //init all enemies have are ACTOR_DEAD, stop enemies from last game
        //from being active when restarting
        map.enemies[i].state = ACTOR_DEAD;
    }
    map.exit_spawn_x = MAP_TILES_X - 2;
    map.exit_spawn_y = 1;
    
    game.current_level += 1;
    game.exit_on_next_frame = false;
    game.game_over_on_next_frame = false;
    game.reset_on_next_frame = false;
    game.music_playing = false;
    game.current_scene = SCENE_DAY_TRANSITION;
    generate_map(&map, &game);

    //start a timer to switch to in game scene after 2 seconds
    new_timer(TIMER_TICKS(2000000), TF_ONE_SHOT, set_scene_in_game);
}

void reset_game() 
{
    game.current_level = 0;
    game.player.hp = PLAYER_START_HP;
    setup();
}

void game_over() 
{
    game.current_scene = SCENE_GAME_OVER;
    game.game_over_on_next_frame = false;
}

int main(void)
{
    //enable CPU interrupts
    init_interrupts();

    //initialize peripherals
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    dfs_init(DFS_DEFAULT_LOCATION);  //load filesystem

    controller_init();
    timer_init();

    //start main loop timers
    new_timer(TIMER_TICKS(TIMER_ONE_SECOND / ANIM_SPEED), TF_CONTINUOUS, update_anim);
    new_timer(TIMER_TICKS(TIMER_ONE_SECOND / 20), TF_CONTINUOUS, update_audio);
    new_timer(TIMER_TICKS(TIMER_ONE_SECOND), TF_CONTINUOUS, reset_debug_ticks);
    new_timer(TIMER_TICKS(TIMER_ONE_SECOND / FPS), TF_CONTINUOUS, redraw);
     
    //initialize audio
    audio = audio_setup( FREQUENCY_44KHZ, 4 );
    audio_write_silence();

    //perform any other game logic setup
    reset_game();

    //read in our spritesheet (look in ./res/spritesheet.png for source png file)
    int fp = dfs_open("/spritesheet.sprite");
    sprite_t *spritesheet= malloc( dfs_size( fp ) );
    dfs_read( spritesheet, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    display_context_t disp = 0;

    //game loop
    while(true)
    {
        handle_input(&game, &map, audio, stop_rumble);

        if (redraw_tick) {
            //grab a render buffer
            while( !(disp = display_lock()) );

            //set the text output color to white initially
            //format here is graphics_set_color(text_background_fill, text_foreground_fill)
            graphics_set_color(WHITE, BLACK);

            //blank the screen
            graphics_fill_screen(disp, BLACK);

            switch (game.current_scene)
            {
                case SCENE_IN_GAME:
                    draw_map(&disp, spritesheet, &map, game.show_debug);
                    draw_enemies(&disp, spritesheet, &map, anim_tick);
                    draw_player(&disp, spritesheet, &game, anim_tick);
                    draw_ui(&disp, spritesheet, &game);
                    draw_debug(&disp, audio);
                    break;
                case SCENE_DAY_TRANSITION:
                    draw_title_screen(&disp, &game);
                    draw_debug(&disp, audio);
                    break;
                case SCENE_GAME_OVER:
                    draw_game_over_screen(&disp, &game);
                    draw_debug(&disp, audio);
                    break;
            }

            //reset anim_tick flag, all draw methods should have advanced the frame by now
            anim_tick = false;

            fps_tick();

            //force backbuffer flip and draw to screen
            display_show(disp);

            //disable redraw until it is reset by FPS timer
            redraw_tick = false;
        }
        
        //buffer audio
        if (buffer_audio) {
            audio_tick( audio );
        }

        switch (game.current_scene)
        {
            case SCENE_IN_GAME:
                move_enemies(&game, &map, audio, stop_rumble); 
                break;
            case SCENE_DAY_TRANSITION:
            case SCENE_GAME_OVER:
                break;
        }
            
        //check for level advance (player on exit) or player dead
        if (game.exit_on_next_frame) { setup(); } 
        else if (game.game_over_on_next_frame) { game_over(); } 
        else if (game.reset_on_next_frame) { reset_game(); }

        //tick main loop instruction for debugging
        ops_tick();
    }

    //release anything that was allocated
    free( spritesheet );
    audio_free( audio );
    timer_close();
    audio = NULL;

    return 0;
}