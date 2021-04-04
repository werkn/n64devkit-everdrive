#include "include/input.h"

//callback is the no arg method use to stop rumble
void handle_input(game_t* game_ref, map_t* map_ref, audio_t* audio_ref, void (*stop_rumble_callback)(int ovfl))
{
    //only capture events when key is released
    controller_scan();
    //get_controllers_present is a bitmasked int, BITWISE AND to find out what controllers
    //are present
    struct controller_data keys_released = get_keys_up();
    
    //find first active controller and exit..., this call is very expensive (~10 FPS @ input sampling of 20 times per second)
    int connected_controllers = get_controllers_present();
    
    if( CONTROLLER_1_INSERTED & connected_controllers) { game_ref->controller_id = 0; }
    else if( CONTROLLER_2_INSERTED & connected_controllers) { game_ref->controller_id = 1; }
    else if( CONTROLLER_3_INSERTED & connected_controllers) { game_ref->controller_id = 2; }
    else if( CONTROLLER_4_INSERTED & connected_controllers) { game_ref->controller_id = 3; }
    else { game_ref->controller_id = -1; }

    game_ref->controller_id = 0;

    switch (game_ref->current_scene)
    {
        case SCENE_IN_GAME:
            //don't process input if its not player turn
            if (game_ref->player_turn && !game_ref->player_doing_action)
            {
                //handle movement
                if ( keys_released.c[game_ref->controller_id].left) 
                {
                    //move 1 tile left
                    move_player(game_ref, map_ref, audio_ref,  -1, 0, stop_rumble_callback);
                }
                else if ( keys_released.c[game_ref->controller_id].right) 
                {
                    //move right
                    move_player(game_ref, map_ref, audio_ref, 1, 0, stop_rumble_callback);
                }
                else if ( keys_released.c[game_ref->controller_id].up) 
                {
                    //move up 
                    move_player(game_ref, map_ref, audio_ref, 0, -1, stop_rumble_callback);
                }
                else if ( keys_released.c[game_ref->controller_id].down) 
                {
                    //move down
                    move_player(game_ref, map_ref, audio_ref, 0, 1, stop_rumble_callback);
                }

                if( keys_released.c[game_ref->controller_id].R)
                {
                    game_ref->show_debug = !game_ref->show_debug;
                }

                if( keys_released.c[game_ref->controller_id].R)
                {
                    map_ref->draw_debug= !map_ref->draw_debug;
                }

                if( keys_released.c[game_ref->controller_id].L)
                {
                    audio_ref->draw_debug = !audio_ref->draw_debug;
                }
                
            }
            break;
        case SCENE_GAME_OVER:
            if( keys_released.c[game_ref->controller_id].Z)
            {
                rumble_start(game_ref->controller_id);
                //callback stop_rumble will be called after 0.5 seconds, only once
                new_timer(TIMER_TICKS(500000), TF_ONE_SHOT, stop_rumble_callback);

                game_ref->reset_on_next_frame = true;
            }
            break;
        case SCENE_DAY_TRANSITION:
            break;
    }

}
