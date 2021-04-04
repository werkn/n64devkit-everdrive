
#include "include/enemy.h"

//where is enemy_t? 
//We don't have a enemy_t, we have player using actor_t struct in game_t
//so look in game_t for enemy decleration
void set_enemy_state(map_t* map_ref, actor_anim_state_t state, u8 enemy_index) 
{
    //set state and update tile_id for correct animation
    map_ref->enemies[enemy_index].state = state;
    switch (state)
    {
        case ACTOR_IDLE:
            if (map_ref->enemies[enemy_index].tag == TAG_ENEMY_A) { map_ref->enemies[enemy_index].tile_id = ENEMY_A_IDLE_START; }
            else if (map_ref->enemies[enemy_index].tag == TAG_ENEMY_B) { map_ref->enemies[enemy_index].tile_id = ENEMY_B_IDLE_START; }
            break;
        case ACTOR_HIT:
            if (map_ref->enemies[enemy_index].tag == TAG_ENEMY_A) { 
                map_ref->enemies[enemy_index].tile_id = ENEMY_A_HIT_START; 
            }
            else if (map_ref->enemies[enemy_index].tag == TAG_ENEMY_B) { 
                map_ref->enemies[enemy_index].tile_id = ENEMY_B_HIT_START; 
            }

            //set enemy_doing_action to prevent any other actions while enemy is animating
            map_ref->enemy_doing_action = true;

            //regardless its an enemy that was hit decrement hp by 1
            if (--map_ref->enemies[enemy_index].hp <= 0) { 
                set_enemy_state(map_ref, ACTOR_DEAD, enemy_index); 
            }
            break;
        case ACTOR_DEAD:
            //clear tag for enemy
            map_ref->tiles[map_ref->enemies[enemy_index].x][map_ref->enemies[enemy_index].y].tag = TAG_NONE;
            map_ref->enemies[enemy_index].state = ACTOR_DEAD;
            map_ref->enemy_doing_action = false;
            break;
        case ACTOR_ATTACKING:
            if (map_ref->enemies[enemy_index].tag == TAG_ENEMY_A) { 
                map_ref->enemies[enemy_index].tile_id = ENEMY_A_ATTACK_START; 
            }
            else if (map_ref->enemies[enemy_index].tag == TAG_ENEMY_B) { 
                map_ref->enemies[enemy_index].tile_id = ENEMY_B_ATTACK_START; 
            }
            
            //set enemy_doing_action to prevent any other actions while enemy is animating
            map_ref->enemy_doing_action = true;
            
            break;
        default:
            break;
    }

    //NOTE:  States are reset in draw_enemies
}

void move_enemies(game_t* game_ref, map_t* map_ref, audio_t* audio_ref, void (*stop_rumble_callback)(int ovfl))
{
    //check its the enemies turn, and the previous enemy is done its attack/movement delay
    if (!game_ref->player_turn && !map_ref->enemy_doing_action)
    {
        //only use non-dead enemies
        if (map_ref->enemies[map_ref->active_enemy_id].state != ACTOR_DEAD) 
        { 
            move_enemy(game_ref, map_ref, audio_ref, map_ref->active_enemy_id++, stop_rumble_callback); 
        } else {
            //skip the dead enemy
            map_ref->active_enemy_id++;
        } 

        //check if we cycled through all enemies
        if (map_ref->active_enemy_id >= map_ref->num_of_enemy) 
        {
            map_ref->active_enemy_id = 0;
            //reset player turn all enemies have moved or attacked            
            game_ref->player_turn = true;
        }
    }
}

void move_enemy(game_t* game_ref, map_t* map_ref, audio_t* audio_ref, u8 enemy_index, void (*stop_rumble_callback)(int ovfl))
{
    //check what is in the tile we want to move into
    u8 dest_x, dest_y;
    dest_x = map_ref->enemies[enemy_index].x;
    dest_y = map_ref->enemies[enemy_index].y;
    bool allow_move = false;
    
    //first move horizontally towards player if were not in the same column
    //next move vertically towards player
    if (game_ref->player.x > map_ref->enemies[enemy_index].x) { dest_x += 1; }
    else if (game_ref->player.x < map_ref->enemies[enemy_index].x) { dest_x -= 1;}
    else if (game_ref->player.y > map_ref->enemies[enemy_index].y) { dest_y += 1; }  
    else if (game_ref->player.y < map_ref->enemies[enemy_index].y) { dest_y -= 1; }  

    tile_tags_t dest_tile_tag = get_tag_at_x_y(map_ref, dest_x, dest_y);
    switch (dest_tile_tag) 
    {
        case TAG_NONE:
            allow_move = true;
            break;
        case TAG_PLAYER:
            set_enemy_state(map_ref, ACTOR_ATTACKING, enemy_index);
            set_player_state(game_ref, ACTOR_HIT);
            audio_play_sfx(audio_ref, SFX_HIT);
            
            rumble_start(game_ref->controller_id);
            //callback stop_rumble will be called after 0.5 seconds, only once
            new_timer(TIMER_TICKS(500000), TF_ONE_SHOT, stop_rumble_callback);
            break;
        case TAG_EXIT:
        case TAG_APPLES:
        case TAG_DRINK:
        case TAG_ENEMY_A:
        case TAG_ENEMY_B:
        case TAG_WALL:
        case TAG_OUTER_WALL:
            allow_move = false;
            break;
    }

    if (allow_move) {

        //remove tag from tile moved from
        map_ref->tiles[map_ref->enemies[enemy_index].x][map_ref->enemies[enemy_index].y].tag = TAG_NONE;

        //move enemy
        map_ref->enemies[enemy_index].x = dest_x;
        map_ref->enemies[enemy_index].y = dest_y;

        //update enemy tag
        map_ref->tiles[dest_x][dest_y].tag = map_ref->enemies[enemy_index].tag;
    }
}

void draw_enemies(display_context_t* disp, sprite_t* spritesheet, map_t* map_ref, bool anim_tick)
{
    //advance frame if elapsed time has passed
    for (u8 i = 0; i < map_ref->num_of_enemy; i++)
    {
        //we should technically free memory here and redefined array if enemy hp <= 0
        //but for now well keep enemy in memory
        //because it can be recycled next level and 
        //just not draw it and clear from map tile_tags
        if (map_ref->enemies[i].state != ACTOR_DEAD) {

            graphics_draw_sprite_trans_stride(*disp, map_ref->enemies[i].x * TILE_WIDTH, map_ref->enemies[i].y * TILE_HEIGHT, spritesheet, map_ref->enemies[i].tile_id);
            if (i == map_ref->active_enemy_id)
            {
                graphics_draw_box(*disp, map_ref->enemies[i].x * TILE_WIDTH, map_ref->enemies[i].y * TILE_HEIGHT, 16, 16, 0x0);
            }
            
            if (anim_tick) { map_ref->enemies[i].tile_id++; }

            switch (map_ref->enemies[i].tag)
            {
                case TAG_ENEMY_A:
                    if (map_ref->enemies[i].state == ACTOR_IDLE && map_ref->enemies[i].tile_id > ENEMY_A_IDLE_END) { set_enemy_state(map_ref, ACTOR_IDLE, i); } 
                    else if (map_ref->enemies[i].state == ACTOR_HIT && map_ref->enemies[i].tile_id > ENEMY_A_HIT_END) {  
                        set_enemy_state(map_ref, ACTOR_IDLE, i); 
                        //clear actor action
                        map_ref->enemy_doing_action = false;
                    } 
                    else if (map_ref->enemies[i].state == ACTOR_ATTACKING && map_ref->enemies[i].tile_id > ENEMY_A_ATTACK_END) { 
                        set_enemy_state(map_ref, ACTOR_IDLE, i); 
                        //clear actor action
                        map_ref->enemy_doing_action = false;
                    } 
                    break;
                case TAG_ENEMY_B:
                    if (map_ref->enemies[i].state == ACTOR_IDLE && map_ref->enemies[i].tile_id > ENEMY_B_IDLE_END) { set_enemy_state(map_ref, ACTOR_IDLE, i); } 
                    else if (map_ref->enemies[i].state == ACTOR_HIT && map_ref->enemies[i].tile_id > ENEMY_B_HIT_END) { 
                        set_enemy_state(map_ref, ACTOR_IDLE, i); 
                        //clear actor action
                        map_ref->enemy_doing_action = false;
                    } 
                    else if (map_ref->enemies[i].state == ACTOR_ATTACKING && map_ref->enemies[i].tile_id > ENEMY_B_ATTACK_END) { 
                        set_enemy_state(map_ref, ACTOR_IDLE, i); 
                        //clear actor action
                        map_ref->enemy_doing_action = false;
                    } 
                    break;
                default:
                    break;
            }
        }    
    }
}