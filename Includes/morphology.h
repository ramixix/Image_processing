#ifndef IMG_MORPHOLOGY_H
#define IMG_MORPHOLOGY_H


#include "pixels.h"

int check_part(struct pixel *pixes, int *structing_image, int width, int structing_image_height, int structing_image_width);
void erision(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width);
void dilation(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width);
void boundary_extraction(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width);
void region_filling(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width);
void kinda_region_filling(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width);
void opening(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width);
void closing(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width);

#endif