#include <stdlib.h>
#include "../Includes/pixels.h"


void rgb_to_gray(struct pixel *pix){
    uint8_t gray = (pix->red + pix->green + pix->blue)/3;
    set_pixel_value(pix, gray, gray, gray);
}


uint16_t get_total_pixel_value(struct pixel pix){
    return (pix.red + pix.green + pix.blue);
}


void set_pixel_value(struct pixel *pix, uint8_t red, uint8_t green, uint8_t blue){
    pix->red = red;
    pix->green = green;
    pix->blue = blue;
}


void set_random_rgb(struct pixel *pix){
    pix->red = ((rand()% 255 + rand()%75 ) % 255);
    pix->green = ((rand()% 255 + rand()%75 ) % 255);
    pix->blue = ((rand()% 255 + rand()%75 ) % 255);
}


void copy_pixel_value(struct pixel *pix1, struct pixel *pix2){
    pix1->red = pix2->red;
    pix1->green = pix2->green;
    pix1->blue = pix2->blue;
}


int compare_two_pixels(struct pixel pix1, struct pixel pix2){
    if( pix1.red == pix2.red && pix1.green == pix2.green && pix1.blue == pix2.blue){
        return 1;
    }
    return 0;
  
}


int not_background(struct pixel pix){
    uint16_t background_value = 0; 
    if(get_total_pixel_value(pix) != background_value){
        return 1;
    }
    return 0;
}
