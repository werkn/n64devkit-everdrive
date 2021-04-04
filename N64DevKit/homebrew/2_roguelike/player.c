#include "include/player.h"

//where is player_t? 
//We don't have a player_t, we have player using actor_t struct in game_t
//so look in game_t for player decleration
void set_player_state(game_t* game_ref, actor_anim_state_t state) 
{
    //set state and update tile_id for correct animation
    game_ref->player.state = state;
    switch (game_ref->player.state) 
    {
        case ACTOR_IDLE:
            game_ref->player.tile_id = PLAYER_IDLE_START;
            
            //player only performs one action per turn right now
            //but using two bool flags allows multiple actions per
            //turns if we add it in the future
            if (game_ref->player_doing_action) {
                game_ref->player_doing_action = false;

                //only end player turn after its finished attacking
                game_ref->player_turn = false;
            }
            break;
        case ACTOR_HIT:
            game_ref->player.tile_id = PLAYER_HIT_START; 
            
            //decrement HP, check if dead
            //note we check whethe result will be > 0, otherwise well have u16 become max value and game over
            //check will fail
            game_ref->player.hp = ((game_ref->player.hp - ENEMY_ATTACK_DMG) > 0) ? (game_ref->player.hp - ENEMY_ATTACK_DMG) : 0 ;

            if (game_ref->player.hp <= 0) { set_player_state(game_ref, ACTOR_DEAD); }
            break;
        case ACTOR_ATTACKING:
            game_ref->player.tile_id = PLAYER_ATTACK_START;
            game_ref->player_doing_action = true;
            break;
        case ACTOR_DEAD:
            game_ref->game_over_on_next_frame = true; 
            game_ref->player_doing_action = false;
            break;
    }
            
}

void draw_player(display_context_t* disp, sprite_t* spritesheet, game_t* game_ref, bool anim_tick)
{
    graphics_draw_sprite_trans_stride(*disp, game_ref->player.x * TILE_WIDTH, game_ref->player.y * TILE_HEIGHT, spritesheet, game_ref->player.tile_id);

    //advance frame if elapsed time has passed
    if (anim_tick) { game_ref->player.tile_id++; }

    switch (game_ref->player.state) 
    {
        case ACTOR_IDLE:
            if (game_ref->player.tile_id > PLAYER_IDLE_END) { set_player_state(game_ref, ACTOR_IDLE); }
            break;
        case ACTOR_HIT:
            if (game_ref->player.tile_id > PLAYER_HIT_END) { set_player_state(game_ref, ACTOR_IDLE); }
            break;
        case ACTOR_ATTACKING:
            if (game_ref->player.tile_id > PLAYER_ATTACK_END) { set_player_state(game_ref, ACTOR_IDLE); }
            break;
        case ACTOR_DEAD:
            break;
    }
}

void move_player(game_t* game_ref, map_t* map_ref, audio_t* audio_ref, u8 dX, u8 dY, void (*stop_rumble_callback)(int ovfl))
{
    //check what is in the tile we want to move into
    u8 dest_x, dest_y;
    bool allow_move = false;
    dest_x = game_ref->player.x + dX;
    dest_y = game_ref->player.y + dY;
    tile_tags_t dest_tile_tag = get_tag_at_x_y(map_ref, dest_x, dest_y);
    switch (dest_tile_tag) 
    {
        case TAG_APPLES:
            game_ref->player.hp += APPLE_HP_RESTORE;
            //despawn apple, reset to ground tile, tag player location
            map_ref->tiles[dest_x][dest_y].tile_id = GND_TILE_START;
            allow_move = true;
            audio_play_sfx(audio_ref, SFX_APPLE);
            break;
        case TAG_DRINK:
            game_ref->player.hp += DRINK_HP_RESTORE;
            map_ref->tiles[dest_x][dest_y].tile_id = GND_TILE_START;
            allow_move = true;
            audio_play_sfx(audio_ref, SFX_SODA);
            break;
        case TAG_PLAYER:
            break;
        case TAG_EXIT:
            allow_move = true;
            game_ref->exit_on_next_frame = true;
            break;
        case TAG_NONE:
            allow_move = true;
            break;
        case TAG_ENEMY_A:
        case TAG_ENEMY_B:
            set_player_state(game_ref, ACTOR_ATTACKING);
            for (u8 i = 0; i < map_ref->num_of_enemy; i++) 
            {
                if (map_ref->enemies[i].x == dest_x && map_ref->enemies[i].y == dest_y)
                {
                    //this will update their state and decrement appropriate amount of hp
                    set_enemy_state(map_ref, ACTOR_HIT, i);
                }
            }
            audio_play_sfx(audio_ref, SFX_PLAYER_CHOP_1); 
            
            rumble_start(game_ref->controller_id);
            //callback stop_rumble will be called after 0.5 seconds, only once
            new_timer(TIMER_TICKS(500000), TF_ONE_SHOT, stop_rumble_callback);
            
            //remove action amount
            game_ref->player.hp -= PLAYER_ACTION_COST;
            break;
        case TAG_WALL:
            set_player_state(game_ref, ACTOR_ATTACKING);
            wall_hit(map_ref, dest_x, dest_y);
            audio_play_sfx(audio_ref, SFX_PLAYER_CHOP_1);
            
            rumble_start(game_ref->controller_id);
            //callback stop_rumble will be called after 0.5 seconds, only once
            new_timer(TIMER_TICKS(500000), TF_ONE_SHOT, stop_rumble_callback);
            
            //remove action amount
            game_ref->player.hp -= PLAYER_ACTION_COST;
            break;
        case TAG_OUTER_WALL:
            allow_move = false;
            break;
    }

    if (allow_move) {

        //remove tag from tile moved from
        map_ref->tiles[game_ref->player.x][game_ref->player.y].tag = TAG_NONE;

        //move player by dX
        game_ref->player.x += dX;
        game_ref->player.y += dY;

        //update player tag
        map_ref->tiles[dest_x][dest_y].tag = TAG_PLAYER;
        
        //play foot audio, commented out here because it sounds terrible
        //audio_play_sfx(audio_ref, SFX_FOOTSTEP_1); 

        //remove action amount
        game_ref->player.hp -= PLAYER_ACTION_COST;

        game_ref->player_turn = false;
    }

    //check if player is performing multi-frame action,
    //if they aren't like when moving or collecting items
    //instantly end turn
    if (!game_ref->player_doing_action) {
        game_ref->player_turn = false;
    }
    

}