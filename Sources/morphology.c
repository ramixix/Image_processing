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


void boundary_extraction(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width){
    struct pixel *boundry_pixes = (struct pixel *)malloc(sizeof(struct pixel) * height * width);
    memset(boundry_pixes, 0, sizeof(struct pixel)*height*width);

    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            copy_pixel_value((boundry_pixes+h*width+w), (pixes+h*width+w));   
        }
    }

    erision(boundry_pixes, structing_image, height, width, structing_img_height, structing_img_width);

    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            if(compare_two_pixels(pixes[h*width+w], boundry_pixes[h*width+w]))
                set_pixel_value(pixes+h*width+w, 0, 0, 0);
        }
    }

    free(boundry_pixes);
}


void region_filling(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width){
    struct pixel *compliment_pixes = (struct pixel *)malloc(sizeof(struct pixel) * height * width);
    memset(compliment_pixes, 0, sizeof(struct pixel)*height*width);

    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            if(get_total_pixel_value(pixes[h*width+w]) == 0)
                set_pixel_value( (compliment_pixes+h*width+w), 255, 255, 255);
        }
    }

    struct pixel *new_pixes = (struct pixel *)malloc(sizeof(struct pixel) * height * width);

    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            copy_pixel_value((new_pixes+h*width+w), (pixes+h*width+w));   
        }
    }

    for(int h=1; h<height-1; h++){
        for(int w=1; w<width-1; w++){
            int result = check_part(pixes+(h-1)*width+(w-1), structing_image, width, structing_img_height, structing_img_width);
            if(result != 0){
                set_pixel_value( (new_pixes+h*width+w), 255, 255, 255 );
            }
            if(compare_two_pixels(new_pixes[h*width+w], compliment_pixes[h*width+w]) == 0){
                set_pixel_value( (new_pixes+h*width+w), 0, 0, 0 );
            }
            
        }
    }

    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            if(get_total_pixel_value(new_pixes[h*width+w])!=0 || get_total_pixel_value(pixes[h*width+w])!=0 ){
                set_pixel_value(pixes+h*width+w, 255, 255, 255);
            }
        }
    }


    // for(int h=0; h<height; h++){
    //     for(int w=0; w<width; w++){
    //         copy_pixel_value((pixes+h*width+w), (new_pixes+h*width+w));   
    //     }
    // }

    free(compliment_pixes);
    free(new_pixes);

        
}


void kinda_region_filling(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width){
    struct pixel *tmp_pixes = (struct pixel *)malloc(sizeof(struct pixel) * height * width);
    memset(tmp_pixes, 0, sizeof(struct pixel)*height*width);

    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            if(not_background(pixes[h*width+w]) == 1)
                set_pixel_value( (tmp_pixes+h*width+w), 255, 255, 255);
        }
    }

    boundary_extraction(pixes, structing_image, height, width, structing_img_height, structing_img_width);
    dilation(pixes, structing_image, height, width, structing_img_height, structing_img_width);

    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            if(not_background(tmp_pixes[h*width+w]) || not_background(pixes[h*width+w]) ){
                set_pixel_value(pixes+h*width+w, 255, 255, 255);
            }
        }
    }

}


void opening(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width){
    erision(pixes, structing_image, height, width, structing_img_height, structing_img_width);
    dilation(pixes, structing_image, height, width, structing_img_height, structing_img_width);
}


void closing(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width){
    dilation(pixes, structing_image, height, width, structing_img_height, structing_img_width);
    erision(pixes, structing_image, height, width, structing_img_height, structing_img_width);
}