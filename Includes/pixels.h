#ifndef PIXLES_H
#define PIXLES_H

#include <stdint.h>

struct pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue; 
};

// struct to detect and take care of the collisions while labeling images.
struct collision{
    struct pixel *left_pix;
    struct pixel *up_pix;
};

void rgb_to_gray(struct pixel *pix);
uint16_t get_total_pixel_value(struct pixel pix);
void set_pixel_value(struct pixel *pix, uint8_t red, uint8_t green, uint8_t blue);
void set_random_rgb(struct pixel *pix);
void copy_pixel_value(struct pixel *pix1, struct pixel *pix2);
int compare_two_pixels(struct pixel pix1, struct pixel pix2);
int not_background(struct pixel pix);

#endif