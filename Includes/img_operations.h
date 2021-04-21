#ifndef IMG_OPERATIONS_H
#define IMG_OPERATIONS_H

#include <stdint.h>
#include "pixels.h"

#define Signature "BM"
#define Bits_Per_Pixel 24
#define NoCompression 0
#define Standart_Headersize 54
#define DataOffset 10

struct Image
{
    unsigned char *header;
    uint32_t headersize;
    uint32_t image_size;
    uint32_t width;
    uint32_t height;
    struct pixel *pixels;
    uint8_t is_gray;
};

int Read_bmp(char* bmp_path, struct Image* img);
void Write_bmp(char *path_to_write, struct Image img);
void convert_to_gray(struct Image *img);
uint32_t *get_histogram(struct Image img);
void convert_to_binary_kmeans(struct Image img);
void change(int bigger_lable, int less_label, int width, int hieght, uint8_t *matrix);
void labeling(struct Image *img);
int Reset_changes(char *path, struct Image* img);
void handel_collision(struct collision collided, struct pixel *pixes, int height, int width);

#endif