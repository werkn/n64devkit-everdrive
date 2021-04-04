#include "include/map.h"

//spawn an item/enemy/wall in random spot
void spawn_random_location(map_t* map_ref, u8 num_to_spawn, u8* tile_list, u8 num_tiles_in_list, bool spawn_enemy)
{
    actor_t new_enemy; 
    bool enemy_already_at_x_y = false;
    for (u8 i = 0; i < num_to_spawn; i++) 
    {
        //select a random tile from available list 
        u8 tile_id =  u8_rand_val(tile_list, num_tiles_in_list);

        //spawn to random location on map (in safe spawn rect, not wall and first tile adjacent to walls)
        u8 spawn_x = u8_rand_range(2, MAP_TILES_X - 3);
        u8 spawn_y = u8_rand_range(2, MAP_TILES_Y - 3);

        if (spawn_enemy)
        {
            //replace whatever is currently there with a floor tile (ie: erase wall or food), unless
            //there is already an enemy there, then simply set its tag to ACTOR_DEAD
            if (map_ref->tiles[spawn_x][spawn_y].tag == TAG_ENEMY_A || 
                map_ref->tiles[spawn_x][spawn_y].tag == TAG_ENEMY_B) {
                //we use this to kill the enemy spawn on top of the other enemy at the end of this method
                enemy_already_at_x_y = true;
            }

            map_ref->tiles[spawn_x][spawn_y].tile_id = GND_TILE_START; 
             
            //setup enemy
            new_enemy.hp = ENEMY_START_HP;
            new_enemy.state = ACTOR_IDLE;
            new_enemy.x = spawn_x;
            new_enemy.y = spawn_y; 
            new_enemy.tile_id = tile_id;

            //spawn an enemy into map struct
            map_ref->enemies[i] = new_enemy; 

        } else {
            //just spawn directly into tiles array
            map_ref->tiles[spawn_x][spawn_y].tile_id = tile_id;
        }

        tile_tags_t tile_tag;
        switch (tile_id) 
        {
            //tag tile for later
            case ENEMY_A_IDLE_START:
                tile_tag = TAG_ENEMY_A;
                map_ref->enemies[i].tag = tile_tag; 
                break;
            case ENEMY_B_IDLE_START:
                tile_tag = TAG_ENEMY_B;
                map_ref->enemies[i].tag = tile_tag; 
                break;
            case APPLES:
                tile_tag = TAG_APPLES;
                break;
            case DRINK:
                tile_tag = TAG_DRINK;
                break;
            case DESTR_WALL_A:
            case DESTR_WALL_A_DMG:
            case DESTR_WALL_B:
            case DESTR_WALL_B_DMG:
                tile_tag = TAG_WALL;
                break;
            default:
                tile_tag = TAG_NONE;
                break;
        }

        //set tag, if enemy isn't already spawn there
        //messy but quick way to handle double spawning on same x,y
        if (enemy_already_at_x_y)
        {
            //also kill this enemy
            map_ref->enemies[i].state = ACTOR_DEAD;
            map_ref->enemies[i].hp = 0;
        } 
        else
        {  
            map_ref->tiles[spawn_x][spawn_y].tag = tile_tag;
        }
    }
}

//method to generate our map
void generate_map(map_t* map_ref, game_t* game_ref) 
{
    //build floor and exterior walls
    u8 ext_wall_count = 2;
    u8 ext_wall_ids[2] = { EXT_WALL_A, EXT_WALL_B };

    for (u8 x = 0; x < MAP_TILES_X; x++) 
    {
        for (u8 y = 0; y < MAP_TILES_Y; y++)
        {
            //if were in the top row, or bottom row of map, fill with walls
            if (x < MAP_TILES_X && (y == 0 || y == MAP_TILES_Y - 1)) { 
                map_ref->tiles[x][y].tile_id = u8_rand_val(ext_wall_ids, ext_wall_count); 
                map_ref->tiles[x][y].tag = TAG_OUTER_WALL;
            }
            //were in either the left most column or right most, fill with walls
            else if  (x == 0 || x == MAP_TILES_X -1) 
            { 
                map_ref->tiles[x][y].tile_id = u8_rand_val(ext_wall_ids, ext_wall_count); 
                map_ref->tiles[x][y].tag = TAG_OUTER_WALL;
            }
            //else its the ground, fill with random ground tile
            else 
            { 
                map_ref->tiles[x][y].tile_id = u8_rand_range(GND_TILE_START, GND_TILE_END); 
                map_ref->tiles[x][y].tag = TAG_NONE;
            }

            //modify exit tile (in case we want to change where it spawns in the future, always top right atm)
            map_ref->tiles[map_ref->exit_spawn_x][map_ref->exit_spawn_y].tile_id = EXIT;
            map_ref->tiles[map_ref->exit_spawn_x][map_ref->exit_spawn_y].tag = TAG_EXIT;
        }
    }
    
    //spawn food
    u8 food_tile_ids[2] = { APPLES, DRINK };
    spawn_random_location(map_ref, u8_rand_range(MIN_FOOD, MAX_FOOD), food_tile_ids, 2, false);

    //spawn walls
    u8 destr_wall_tile_ids[4] = { DESTR_WALL_A, DESTR_WALL_A_DMG, DESTR_WALL_B, DESTR_WALL_B_DMG };
    spawn_random_location(map_ref, u8_rand_range(MIN_DESTR_WALL, MAX_DESTR_WALL), destr_wall_tile_ids, 4, false);
    
    //limit to max of 10 enemies
    map_ref->num_of_enemy = log2f(game_ref->current_level);
   
    u8 enemy_tile_ids[2] = { ENEMY_A_IDLE_START, ENEMY_B_IDLE_START };
    spawn_random_location(map_ref, map_ref->num_of_enemy, enemy_tile_ids, 2, true);

    //lastly tag player spawn point
    map_ref->tiles[game_ref->player_spawn_x][game_ref->player_spawn_y].tag = TAG_PLAYER;
}

void draw_map(display_context_t* disp, sprite_t* spritesheet, map_t* map_ref, bool draw_tags)
{
    u8 x,y;
    for (x = 0; x < MAP_TILES_X; x++) 
    {
        for (y = 0; y < MAP_TILES_Y; y++)
        {
            graphics_draw_sprite_stride(*disp, x*TILE_WIDTH, y*TILE_HEIGHT, spritesheet, map_ref->tiles[x][y].tile_id);
            if (draw_tags) 
            {
                //draw the tag for the current tile
                switch (map_ref->tiles[x][y].tag)
                {
                    case TAG_NONE:
                        graphics_draw_text(*disp, x*TILE_WIDTH, y*TILE_HEIGHT, " N");
                        break;
                    case TAG_APPLES:
                        graphics_draw_text(*disp, x*TILE_WIDTH, y*TILE_HEIGHT, " A");
                        break;
                    case TAG_DRINK:
                        graphics_draw_text(*disp, x*TILE_WIDTH + (TILE_WIDTH/2), y*TILE_HEIGHT, " DR");
                        break;
                    case TAG_ENEMY_A:
                        graphics_draw_text(*disp, x*TILE_WIDTH, y*TILE_HEIGHT, " EA");
                        break;
                    case TAG_ENEMY_B:
                        graphics_draw_text(*disp, x*TILE_WIDTH, y*TILE_HEIGHT, " EB");
                        break;
                    case TAG_PLAYER:
                        graphics_draw_text(*disp, x*TILE_WIDTH, y*TILE_HEIGHT, " P");
                        break;
                    case TAG_WALL:
                        graphics_draw_text(*disp, x*TILE_WIDTH, y*TILE_HEIGHT, " W");
                        break;
                    case TAG_OUTER_WALL:
                        graphics_draw_text(*disp, x*TILE_WIDTH, y*TILE_HEIGHT, " OW");
                        break;
                    case TAG_EXIT:
                        graphics_draw_text(*disp, x*TILE_WIDTH, y*TILE_HEIGHT, "EXIT");
                        break;
                }
            }
        }
    }
}

//player has hit a wall, update sprite, or destroy if last DMG sprite
void wall_hit(map_t* map_ref, u8 x, u8 y)
{
    u8 wall_tile_id = map_ref->tiles[x][y].tile_id;

    switch (wall_tile_id)
    {
        case DESTR_WALL_A:
            map_ref->tiles[x][y].tile_id = DESTR_WALL_A_DMG;
            break;
        case DESTR_WALL_B:
            map_ref->tiles[x][y].tile_id = DESTR_WALL_B_DMG;
            break;
        case DESTR_WALL_A_DMG:
        case DESTR_WALL_B_DMG:
            map_ref->tiles[x][y].tile_id = GND_TILE_START;
            map_ref->tiles[x][y].tag = TAG_NONE;
            break;
    }
}
//will return whats at x,y coord, if an enemy is at the coord it will return the enemy tag
//possible values returned are:
//  - PLAYER, ENEMY_A, ENEMY_B, APPLES, DRINK, WALL, NONE
tile_tags_t get_tag_at_x_y(map_t* map_ref, u8 x, u8 y)
{
    //TODO: make sure we are not going outside bounds of array for map
    return map_ref->tiles[x][y].tag;
}