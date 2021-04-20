#ifndef PIXLES_H
#define PIXLES_H

#include <stdint.h>

struct pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue; 
};

void rgb_to_gray(struct pixel *pix);
uint16_t get_total_pixel_value(struct pixel *pix);
void set_pixel(struct pixel *pix, uint8_t red, uint8_t green, uint8_t blue);
void set_random_rgb(struct pixel *pix);
void copy_pix_value(struct pixel *pix1, struct pixel *pix2);
int check_pixs_value(struct pixel *pix1, struct pixel *pix2);

#endif