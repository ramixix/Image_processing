#include <stdlib.h>
#include "../Includes/pixels.h"


// setting pixel to gray, calculating mean of red, green and blue values by adding them together and then deviding total by 3
// set all pixel values to calculated mean(gray value).
void rgb_to_gray(struct pixel *pix){
    uint8_t gray = (pix->red + pix->green + pix->blue)/3;
    set_pixel_value(pix, gray, gray, gray);
}


// return total pixel value by adding red, green and blue values,
uint16_t get_total_pixel_value(struct pixel pix){
    return (pix.red + pix.green + pix.blue);
}


// Set a pixel red, green and blue values to specified values
void set_pixel_value(struct pixel *pix, uint8_t red, uint8_t green, uint8_t blue){
    pix->red = red;
    pix->green = green;
    pix->blue = blue;
}


// Set a pixel red, green and blue values to random values
void set_random_rgb(struct pixel *pix){
    pix->red = ((rand()% 255 + rand()%75 ) % 255);
    pix->green = ((rand()% 255 + rand()%75 ) % 255);
    pix->blue = ((rand()% 255 + rand()%75 ) % 255);
}


// Copy red, green and blue values one pixel to other (pix2 to pix1)
void copy_pixel_value(struct pixel *pix1, struct pixel *pix2){
    pix1->red = pix2->red;
    pix1->green = pix2->green;
    pix1->blue = pix2->blue;
}


// compare two pixels by comparing their red, green and blue values. return 1 if they are all equal, otherwise return 0
int compare_two_pixels(struct pixel pix1, struct pixel pix2){
    if( pix1.red == pix2.red && pix1.green == pix2.green && pix1.blue == pix2.blue){
        return 1;
    }
    return 0;
  
}


// chech if a pixel's red, green and blue value is equal to 0 ( 0 specify the background in our programm).
// return 1 if it is not background pixel otherwise return 0
int not_background(struct pixel pix){
    uint16_t background_value = 0; 
    if(get_total_pixel_value(pix) != background_value){
        return 1;
    }
    return 0;
}
