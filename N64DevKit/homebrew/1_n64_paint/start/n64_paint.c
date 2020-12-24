#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdint.h>
#include <libdragon.h>

static volatile uint32_t animcounter = 0;

void update_counter( int ovfl )
{
    animcounter++;
}

int main(void)
{
    /* enable interrupts (on the CPU) */
    init_interrupts();

    /* Initialize peripherals */
    display_init( RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, ANTIALIAS_RESAMPLE );
    rdp_init();
    controller_init();
    timer_init();

    /* Kick off animation update timer to fire thirty times a second */
    new_timer(TIMER_TICKS(1000000 / 30), TF_CONTINUOUS, update_counter);

    /* Main loop test */
    while(1)
    {
        static display_context_t disp = 0;

        /* Grab a render buffer */
        while( !(disp = display_lock()) );

        /* Fill the screen */
        graphics_fill_screen( disp, 0xFFFFFFFF );

        /* Set the text output color */
        graphics_set_color( 0x0, 0xFFFFFFFF );

        /* To do initialize routines */
        controller_scan();
        struct controller_data keys = get_keys_down();

        if( keys.c[0].A )
        {
        }

        graphics_draw_text(disp, 20, 20, "Hello World!");

        /* Force backbuffer flip, ie: draw to screen */
        display_show(disp);
    }
}
