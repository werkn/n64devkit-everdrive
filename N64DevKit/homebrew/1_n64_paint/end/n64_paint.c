/**
* @author:  werkn / Ryan Radford 
* werkn.github.io / https://www.youtube.com/channel/UCg7jWtPBdmf4grhAbBDkffw
*
* I present to you the premiere graphic editing suite for 2020.  
* Photoshop and GIMP killer for sure :)
*
* This app shows a lot of the basic techniques needed to start working
* with the Libdragon library to develop your own homebrew on the N64.  
* This app is part of a tutorial series I'm working on found at: 
* https://youtube.com/playlist?list=PLtQmMQGpPR6I8mpCemyRqRbxpwHKYsXYt
*
* LICENSE:  Do what you want with it, but if you do something cool
* I'd love to see it in a pull request on the repo this file was found in :)
*
* NOTE:  I'm not a c developer, pretty much figured out how to write c as I
* made this little app, things I did probably suck / aren't optimized but I 
* tend to not finish projects if I don't move quickly and release it, so I'm
* doing that now.  May fix things up here as I improve in c.
*
* Thats all, be awesome my dudes!
* -------------------------------
* TOP SECRET:
* aHR0cHM6Ly93d3cueW91dHViZS5jb20vd2F0Y2g/dj1aWjVMcHdPLUFuNA==
* -------------------------------
*/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

//color palette 
uint32_t WHITE, BLACK, RED, GREEN, BLUE, GRAY;

enum tool {
    BRUSH = 15, //brush, index is its sprite id
    TRASH = 16 //eraser
};

//used for animation timing
static volatile uint32_t animcounter = 0;

//stores whats been drawn to canvas, we could use a stack implementation here
//and only save once drawing but meh, this was quicker to implement
static int canvas[30][15];

//struct to hold our color palette (ie: colors used in the UI color slots)
static struct palette
{
    int index;  //which color block we are currently on
    uint32_t palette_slots[5];  //palette slot colors at bottom of screen
} palette;

//struct for storing app state stuff
static struct paint_app
{
    int cursor_x; 
    int cursor_y;
    int controller_id;  //if (0 - 3) assigned active, -1 indicates disconnected
    enum tool active_tool;  //what tool were using (brush or eraser/trash)
    bool explode_canvas; //flag for playing explosions
} paint_app;

void update_counter( int ovfl )
{
    animcounter++;
}

//no arg callback to be used with timer to stop rumbling after 1 second
void stop_rumble()
{
    rumble_stop(paint_app.controller_id);
}

//clear the canvas back to palette slot 0 color
void reset_canvas() 
{
    int x,y;
    for(x = 0; x < 30; x++) {
        for (y = 0; y < 15; y++) {
            canvas[x][y] = 3; //initially assign palette index 3, ie: WHITE
        }
    } 
}

void setup() 
{
    //cursor_x/y is for canvas space (ie: 0,0 in canvas space is actually 10,20 in screen coords)
    paint_app.cursor_x = 0;
    paint_app.cursor_y = 0;
    paint_app.controller_id = -1;
    paint_app.active_tool = BRUSH;
    paint_app.explode_canvas = false;
    palette.index = 0;

    //NOTE: We are in DEPTH_16_BPP so the method graphics_make_color is used to
    //generate 32-bit representation of an RGBA color
    WHITE = graphics_make_color(255, 255, 255, 255);
    BLACK = graphics_make_color(0, 0, 0, 255);
    RED = graphics_make_color(255, 0, 0, 255);
    GREEN = graphics_make_color(0, 255, 0, 255);
    BLUE = graphics_make_color(0, 0, 255, 255);
    GRAY = graphics_make_color(60, 60, 60, 255);
    palette.palette_slots[0] = RED;
    palette.palette_slots[1] = GREEN;
    palette.palette_slots[2] = BLUE;
    palette.palette_slots[3] = WHITE;
    palette.palette_slots[4] = BLACK;

    reset_canvas();
}

//plays our explosion animation
void play_explosion(display_context_t* disp, sprite_t* spritesheet) 
{
    //don't flame on static usage :) here, new to c and did because of this stackoverflow post
    //https://stackoverflow.com/questions/5033627/static-variable-inside-of-a-function-in-c
    static int explosion_frame = 17;  //only assigned once

    //start explosion
    //advance frame via timer
    //when done, flag explosion done (first explosion is drawn @ cursor location)
    graphics_draw_sprite_trans_stride(*disp, paint_app.cursor_x,  paint_app.cursor_y + 8, spritesheet,  explosion_frame++ );
    graphics_draw_sprite_trans_stride(*disp, 30, 50, spritesheet, explosion_frame++ ); //explosion 1
    graphics_draw_sprite_trans_stride(*disp, 280, 75, spritesheet,  explosion_frame++ ); //explosion 2
    graphics_draw_sprite_trans_stride(*disp, 160, 120, spritesheet,  explosion_frame++ ); //eplosion 3
    if (explosion_frame > 31) //expl last frame is 31 
    { 
        explosion_frame = 17; //expl start frame is 17
        paint_app.explode_canvas = false; 
    }
}

void draw_ui(display_context_t* disp, sprite_t* spritesheet) 
{
    //blank the screen
    graphics_fill_screen(*disp, BLACK);

    //redraw our canvas
    int x,y;
    for(x = 0; x < 30; x++) {
        for (y = 0; y < 15; y++) {
            graphics_draw_box(*disp, 10 + x*10, 20 + y*10, 10, 10, palette.palette_slots[canvas[x][y]]);
        }
    } 

    //draw palette area    
    graphics_draw_box(*disp, 5, 175, 310, 60, WHITE);    
    graphics_draw_box(*disp, 10, 180, 300, 50, GRAY);

    //draw available colors
    graphics_draw_box(*disp, 20, 190, 40, 30, RED);    
    graphics_draw_box(*disp, 60, 190, 40, 30, GREEN);    
    graphics_draw_box(*disp, 100, 190, 40, 30, BLUE);    
    graphics_draw_box(*disp, 140, 190, 40, 30, WHITE);    
    graphics_draw_box(*disp, 180, 190, 40, 30, BLACK);    
    
    //change background color of tools based on which is active
    if (paint_app.active_tool == BRUSH) { 
        graphics_draw_box(*disp, 220, 190, 40, 30, GREEN); //brush tool background 
        graphics_draw_box(*disp, 260, 190, 40, 30, WHITE); //trash tool background   
    }
    else 
    { 
        graphics_draw_box(*disp, 220, 190, 40, 30, WHITE); 
        graphics_draw_box(*disp, 260, 190, 40, 30, GREEN);    
    }

    //set the text output color to white initially
    //format here is graphics_set_color(text_background_fill, text_foreground_fill)
    graphics_set_color(GREEN, BLACK);
    graphics_draw_text(*disp, 117, 10, "n64paint.exe");

    //draw icons
    //brush and A button
    graphics_draw_sprite_trans_stride(*disp, 220, 190, spritesheet, 15); //brush
    graphics_draw_sprite_trans_stride(*disp, 240, 200, spritesheet, 11); //a 
    //trash and B button
    graphics_draw_sprite_trans_stride(*disp, 260, 190, spritesheet, 16); //trash
    graphics_draw_sprite_trans_stride(*disp, 280, 200, spritesheet, 12); //b

    //player icon, the way our sprite sheet is configured is 1st sprite is player 1 icon
    //so to allow us to use only controller_id (ie: controller_id = 0/player1 icon) we simply
    //add this to 1... if we are less then 1, ie: controller_index -1, we use the ? icon for no
    //input binding, which we've ensured has texture index 0
    int palette_offset = 40 * palette.index; //spacing between palette slots in ui
    graphics_draw_sprite_trans_stride(*disp, 25 + palette_offset, 190, spritesheet,  1 + paint_app.controller_id);
    //L/R buttons
    graphics_draw_sprite_trans_stride(*disp, 5  + palette_offset, 210, spritesheet, 13);
    graphics_draw_sprite_trans_stride(*disp, 45  + palette_offset, 210, spritesheet, 14);
    //draw cursor/brush, 10 + paint_app.cursor_x, etc.. is because cursor x is only concerned with canvas space
    graphics_draw_sprite_trans_stride(*disp, 10 + paint_app.cursor_x, 20 + paint_app.cursor_y - 32, spritesheet, paint_app.active_tool);
    graphics_draw_box(*disp, 10 + paint_app.cursor_x, 20 + paint_app.cursor_y, 10, 10, palette.palette_slots[palette.index]);    

    //alert user when no input devices (ie: controllers are found indicated by -1)
    if (paint_app.controller_id == -1) 
    {
        graphics_set_color(WHITE, RED);
        graphics_draw_text(*disp, 25, 120, "No controllers connected!");

    } 

    if (identify_accessory(paint_app.controller_id) == ACCESSORY_RUMBLEPAK) { graphics_draw_text(*disp, 220, 10, "RUMBLE ON"); }
    else { graphics_draw_text(*disp, 220, 10, "RUMBLE OFF"); }

    //play explosion when trash can is used
    if (paint_app.explode_canvas)
    {
        play_explosion(disp, spritesheet);
    }
}

void handle_input()
{
    controller_scan();
    
    struct controller_data keys_held = get_keys_held();

    //only capture events when key is released
    struct controller_data keys_released = get_keys_up();

    //get_controllers_present is a bitmasked int, BITWISE AND to find out what controllers
    //are present
    //find first active controller and exit...
    int connected_controllers = get_controllers_present();

    if( CONTROLLER_1_INSERTED & connected_controllers) { paint_app.controller_id = 0; }
    else if( CONTROLLER_2_INSERTED & connected_controllers) { paint_app.controller_id = 1; }
    else if( CONTROLLER_3_INSERTED & connected_controllers) { paint_app.controller_id = 2; }
    else if( CONTROLLER_4_INSERTED & connected_controllers) { paint_app.controller_id = 3; }
    else { paint_app.controller_id = -1; }

    //handle single fire events, ie: button pressed to change state 
    if( keys_released.c[paint_app.controller_id].L )
    {
        //move selected color in ui back one slot
        palette.index -= 1; 
    }
    else if( keys_released.c[paint_app.controller_id].R )
    {
        //move selected color in ui forward one slot
        palette.index += 1; 
    }

    //handle tool switching
    if( keys_released.c[paint_app.controller_id].A )
    {
        paint_app.active_tool = BRUSH;
    }
    else if( keys_released.c[paint_app.controller_id].B )
    {
        paint_app.active_tool = TRASH;
    }

    //make sure palette index is between 0 and 4 here, change this to % based version once
    //we figure out how to do that in c, we only have have 5 palette slots
    if (palette.index < 0) 
    {
        palette.index = 4;
    } else if (palette.index > 4) {
        palette.index = 0;
    }

    //handle duration fire events, ie: contious movement as button is held down
    //in this context 
    //handle movement
    if ( keys_held.c[paint_app.controller_id].left) 
    {
        //move left
        paint_app.cursor_x -= 10;
    }
    if ( keys_held.c[paint_app.controller_id].right) 
    {
        //move right
        paint_app.cursor_x += 10;
    }
    if ( keys_held.c[paint_app.controller_id].up) 
    {
        //move up 
        paint_app.cursor_y -= 10;
    }
    if ( keys_held.c[paint_app.controller_id].down) 
    {
        //move down
        paint_app.cursor_y += 10;
    }

    //constrain movement to canvas area only
    if (paint_app.cursor_x > 290) { paint_app.cursor_x = 290; } 
    else if (paint_app.cursor_x < 0) { paint_app.cursor_x = 0; } 
    
    if (paint_app.cursor_y > 140) { paint_app.cursor_y = 140; } 
    else if (paint_app.cursor_y < 0) { paint_app.cursor_y = 0; } 

    //handle z-button press for draw to screen, translate screen cursor position to 
    //our canvas array.  If we change movement of cursor (ie: from 10 above, we need to update 10 here)
    //if we refactor put this in a header via something like a const
    if ( keys_held.c[paint_app.controller_id].Z && paint_app.active_tool == BRUSH) 
    {
        canvas[(0 + paint_app.cursor_x) / 10][(0 + paint_app.cursor_y) / 10] = palette.index;
    }

    //handle z-released and tool is trash can
    if( keys_released.c[paint_app.controller_id].Z && paint_app.active_tool == TRASH)
    {
        paint_app.explode_canvas = true;
        rumble_start(paint_app.controller_id);
        //callback stop_rumble will be called after 0.5 seconds, only once
        new_timer(TIMER_TICKS(500000), TF_ONE_SHOT, stop_rumble);
        reset_canvas();
    }

}

int main(void)
{
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init( RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    dfs_init(DFS_DEFAULT_LOCATION);  //load filesystem
    rdp_init(); //we don't make use of the rdp here but left so I don't forget to do in future example
    controller_init();
    timer_init();

    /* Kick off animation update timer to fire thirty times a second */
    new_timer(TIMER_TICKS(1000000 / 30), TF_CONTINUOUS, update_counter);

    //perform any other game logic setup
    setup();

    //read in our spritesheet (icons for n64paint)
    int fp = dfs_open("/spritesheet.sprite");
    sprite_t *icons= malloc( dfs_size( fp ) );
    dfs_read( icons, 1, dfs_size( fp ), fp );
    dfs_close( fp );

    //game loop
    while(1)
    {
        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );

        //set the text output color to white initially
        //format here is graphics_set_color(text_background_fill, text_foreground_fill)
        graphics_set_color(WHITE, BLACK);

        handle_input();
        draw_ui(&disp, icons);

        /* Force backbuffer flip, ie: draw to screen */
        display_show(disp);
    }
}
