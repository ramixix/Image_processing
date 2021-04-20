#include <stdlib.h>
#include <time.h>
#include "../Includes/pixels.h"

void rgb_to_gray(struct pixel *pix){
    uint8_t gray = (pix->red + pix->green + pix->blue)/3;
    pix->red = gray;
    pix->green = gray;
    pix->blue = gray;

}

uint16_t get_total_pixel_value(struct pixel *pix){
    return (pix->red + pix->green + pix->blue);
}

void set_pixel(struct pixel *pix, uint8_t red, uint8_t green, uint8_t blue){
    pix->red = red;
    pix->green = green;
    pix->blue = blue;
}

void set_random_rgb(struct pixel *pix){
    srand(time(0));
    pix->red = ((rand()% 255 )+ 75) % 255;
    pix->green = ((rand()% 255 )+ 75) % 255;
    pix->blue = ((rand()% 255 )+ 75) % 255;
}

void copy_pix_value(struct pixel *pix1, struct pixel *pix2){
    pix2->red = pix1->red;
    pix2->green = pix1->green;
    pix2->blue = pix1->blue;
}

int check_pixs_value(struct pixel *pix1, struct pixel *pix2){
    if( pix1->red == pix2->red && pix1->green == pix2->green && pix1->blue == pix2->blue){
        return 1;
    }
    return 0;
}