#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../Includes/morphology.h"


int check_part(struct pixel *pixes, int *structing_image, int width, int structing_image_height, int structing_image_width){
    int total=0;
    int multipy_result=0;
    for(int h=0; h<structing_image_height; h++){
        for(int w=0; w<structing_image_width; w++){
            multipy_result = get_total_pixel_value(pixes[h*width+w]) * structing_image[h*structing_image_width+w];
            if(multipy_result != 0){
                total +=1;
            }
        }
    }
    return total;
}


void erision(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width){
    struct pixel *new_pixes = (struct pixel *)malloc(sizeof(struct pixel) * height * width);
    memset(new_pixes, 0, sizeof(struct pixel)*height*width);
    for(int h=1; h<height-1; h++){
        for(int w=1; w<width-1; w++){
            int result = check_part(pixes+(h-1)*width+(w-1), structing_image, width, structing_img_height, structing_img_width);
            if(result == 9){
                set_pixel_value( (new_pixes+h*width+w), 255, 255, 255);
            }
        }
    }
    for(int h=1; h<height-1; h++){
        for(int w=1; w<width-1; w++){
            copy_pixel_value( (pixes+h*width+w), (new_pixes+h*width+w) );   
        }
    }
    free(new_pixes);
}

void dilation(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width){
    struct pixel *new_pixes = (struct pixel *)malloc(sizeof(struct pixel) * height * width);
    memset(new_pixes, 0, sizeof(struct pixel)*height*width);
    for(int h=1; h<height-1; h++){
        for(int w=1; w<width-1; w++){
            int result = check_part(pixes+(h-1)*width+(w-1), structing_image, width, structing_img_height, structing_img_width);
            if(result != 0){
                set_pixel_value( (new_pixes+h*width+w), 255, 255, 255 );
            }
            
        }
    }
    for(int h=1; h<height-1; h++){
        for(int w=1; w<width-1; w++){
            copy_pixel_value( (pixes+h*width+w), (new_pixes+h*width+w) );   
        }
    }
    free(new_pixes);
}