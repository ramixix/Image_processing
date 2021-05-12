#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>


struct pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue; 
};

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


void copy_pixel_value(struct pixel *pix1, struct pixel pix2){
    // printf("pix2 : %d ,%d , %d", pix2->blue, pix2->green, pix2->red);
    pix1->red = pix2.red;
    pix1->green = pix2.green;
    pix1->blue = pix2.blue;
}


int compare_two_pixels(struct pixel pix1, struct pixel pix2){
    if( pix1.red == pix2.red && pix1.green == pix2.green && pix1.blue == pix2.blue){
        return 1;
    }
    return 0;
  
}


int not_background(struct pixel pix){
    uint16_t background_value = 0; // red + green + blue = 255 + 255 + 255 = 765
    if(get_total_pixel_value(pix) != background_value){
        return 1;
    }
    return 0;
}


int check_part(struct pixel *pixes, int *structing_image, int structing_image_height, int structing_image_width){
    int total=0;
    int multipy_result=0;
    for(int h=0; h<structing_image_height; h++){
        for(int w=0; w<structing_image_width; w++){
            multipy_result = get_total_pixel_value(pixes[h*5+w]) * structing_image[h*structing_image_width+w];
            if(multipy_result != 0){
                total +=1;
            }
        }
    }
    return total;
}

// void erision(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width){
//     int total;
//     for(int h=1; h<height-1; h++){
//         for(int w=1; w<width-1; w++){
//             if(not_background(pixes[h*width+w])){
//                 total=0;
//                 for(int s_h=0; s_h<structing_img_height; s_h++){
//                     for(int s_w=0; s_w<structing_img_width; s_w++){
//                         int multiply_result=0;
//                         int starting_height = h-1;
//                         int starting_widht = w-1;
//                         multiply_result = get_total_pixel_value( pixes[ (starting_height+s_h)*width+(starting_widht+s_w)] ) * structing_image[s_h*structing_img_width+s_w];
//                         // printf("%d ", multiply_result);
//                         if(multiply_result!=0){
//                             total+=1;
//                         }
//                     }
//                     // printf("\n");
//                 }
//                 printf("%d\n ", total);
//                 if(total != 9){
//                     set_pixel_value( (pixes+h*width+w), 254, 254, 254);
//                 }
//                 else{
//                     set_pixel_value( (pixes+h*width+w), 255, 255, 255);
//                 }

//             }

           
//         }
//     }
void erision(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width){
    for(int h=1; h<height-1; h++){
        for(int w=1; w<width-1; w++){
            int result = check_part(pixes+(h-1)*width+(w-1), structing_image, structing_img_height, structing_img_width);
            if(result != 9){
                set_pixel_value( (pixes+h*width+w), 254, 254, 254);
            }
        }
    }
    for(int h=1; h<height-1; h++){
        for(int w=1; w<width-1; w++){
            if(get_total_pixel_value(pixes[h*width+w]) == 762){
                set_pixel_value(pixes+h*width+w, 0 ,0 ,0);
            }
        }
    }
}

void dilation(struct pixel *pixes, int *structing_image, int height, int width, int structing_img_height, int structing_img_width){
    struct pixel *new_pixes = (struct pixel *)malloc(sizeof(struct pixel)*height*width);
    memset(new_pixes, 0, sizeof(struct pixel)*height*width);
    for(int h=1; h<height-1; h++){
        for(int w=1; w<width-1; w++){
            int result = check_part(pixes+(h-1)*width+(w-1), structing_image, structing_img_height, structing_img_width);
            if(result != 0){
                set_pixel_value( (new_pixes+h*width+w), 255, 255, 255);
            }
        }
    }
    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            printf("%d\t", get_total_pixel_value(new_pixes[h*width+w]));
        }
        printf("\n");
    }printf("\n\n");
    for(int h=1; h<height-1; h++){
        for(int w=1; w<width-1; w++){
            copy_pixel_value(pixes+h*width+w, new_pixes[h*width+w]);
        }
    }
    free(new_pixes);
}

int main(){
    int height = 5;
    int width =5;
    struct pixel image[] = {  {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},{0,0,0}, {255,255,255}, {0,0,0}, {0,0,0}, {0,0,0},{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0},{0,0,0}, {0,0,0}, {0,0,0}, {255,255,255}, {0,0,0},{0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}};
    int foreground = 1;
    int structing_image[] = {foreground, foreground, foreground, foreground, foreground, foreground, foreground, foreground, foreground};
    
    // erision(image, structing_image, height, width, 3, 3);
    dilation(image, structing_image, height, width, 3, 3);

    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            printf("%d\t", get_total_pixel_value(image[h*width+w]));
        }
        printf("\n");
    }

    return 0;
}