#include "include/utils.h"

//get a random integer between min/max
u8 u8_rand_range(u8 min, u8 max)
{
    return rand() % (max + 1 - min) + min;    
}

//given an int array grab a index from it
//allows us to select from an array of differing tile ids
u8 u8_rand_val(u8* values, u8 length)
{
    return values[u8_rand_range(0, length-1)];
}

//get the approx width of text in pixels
u16 text_width(u16 num_of_char, u16 px_per_char) 
{
    return num_of_char * px_per_char;
}

//get the approx x position to center text on screen
u16 center_text_x(u16 screen_width, u16 num_of_char, u16 px_per_char) 
{
    return (screen_width - text_width(num_of_char, px_per_char)) / 2;     
}

