#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue; 
};

struct collision{
    struct pixel *left_pix;
    struct pixel *up_pix;
};

uint16_t get_total_pixel_value(struct pixel pix){
    return (pix.red + pix.green + pix.blue);
}

void set_random_rgb(struct pixel *pix){
    pix->red = ((rand()% 255 )+ 75) % 255;
    pix->green = ((rand()% 255 )+ 75) % 255;
    pix->blue = ((rand()% 255 )+ 75) % 255;
}


void set_pixel(struct pixel *pix, uint8_t red, uint8_t green, uint8_t blue){
    pix->red = red;
    pix->green = green;
    pix->blue = blue;
}

void copy_pix_value(struct pixel *pix1, struct pixel *pix2){
    pix2->red = pix1->red;
    pix2->green = pix1->green;
    pix2->blue = pix1->blue;
}

int check_pixs_value(struct pixel pix1, struct pixel pix2){
    if( pix1.red == pix2.red && pix1.green == pix2.green && pix1.blue == pix2.blue){
        return 1;
    }
    return 0;
}

int not_background(struct pixel pix){
    uint16_t background_value = 765;
    if(get_total_pixel_value(pix) == background_value){
        return 0;
    }
    return 1;
}





void handle_collision(struct collision colli, struct pixel *pixes){
    struct pixel left;
    struct pixel up;
    copy_pix_value(colli.left_pix, &left);
    copy_pix_value(colli.up_pix, &up);
    uint16_t left_value = get_total_pixel_value(left);
    uint16_t up_value = get_total_pixel_value(up);
    struct pixel substitute, throw;
    if(left_value > up_value){
        copy_pix_value(&left, &throw);
        copy_pix_value(&up, &substitute);
    }
    copy_pix_value(&up, &throw);
    copy_pix_value(&left, &substitute);

    for(int h=0; h < 6; h++){
        for(int w=0; w < 6; w++){
            if(check_pixs_value(pixes[h*6 + w], throw)){
                copy_pix_value(&substitute , pixes + h*6 + w);
            }
        }
    }
    // int left1 = get_total_pixel_value(&left);
    // int up1 = get_total_pixel_value(&up);
    // if(left1 > up1){
    //     printf("replacement : %d\n", up1);
    //     printf("overthrow : %d\n", left1);
    //     for(int h=0; h< 5; h++){
    //         for(int w=0; w < 5 ; w++){
    //             if( get_total_pixel_value(pixes + h*5 + w) == left1 ){
    //                 copy_pix_value(&up, pixes + h*5 + w);
    //             }
    //         }
    //     }
        
    // }
    // else{
    //     printf("replacement : %d\n", left1);
    //     printf("overthrow : %d\n", up1);
    //     for(int h=0; h< 5; h++){
    //         for(int w=0; w < 5 ; w++){
    //             if( get_total_pixel_value(pixes + h*5 + w) == up1 ){
    //                 copy_pix_value(&left, pixes + h*5 + w);
    //             }
    //         }
    //     }
        
    // }
    // printf("replacement : %d\n", get_total_pixel_value(replacement));
    // printf("overthrow : %d\n", get_total_pixel_value(overthrow));
    // for(int h=0; h< 4; h++){
    //     for(int w=0; w < 4 ; w++){
    //         if( get_total_pixel_value(pixes + h*4 + w) == get_total_pixel_value(overthrow) ){
    //             copy_pix_value(replacement, pixes + h*4 + w);
    //         }
    //     }
    // }
}



void labeling(struct pixel *pixes){
    int width = 6;
    int height = 6;
    int collision_count=0;
    int size = 1;
    struct collision *colli = (struct collision *)malloc(sizeof(struct collision)* size);
    for(int h=0; h< height; h++){
        for(int w=0; w < width ; w++){
            if( not_background(pixes[h*width + w]) ){
                if( w==0 ){
                    set_random_rgb(pixes + h*width );
                }
                else if( not_background(pixes[h*width + w-1])){
                    copy_pix_value(pixes + h*width + w-1, pixes + h*width + w);
                }
                else{
                    set_random_rgb(pixes + h*width + w );
                }
            }
        }
    }

    for(int h=1; h < height; h++){
        for(int w=0; w < width; w++){
            if( not_background(pixes[h*width + w]) ){
                if( w != 0){
                    if(not_background(pixes[(h-1)*width +w]) && not_background(pixes[h*width + w-1])){
                        if( check_pixs_value(pixes[(h-1)*width + w], pixes[h*width + w-1]) ){
                            copy_pix_value(pixes + h*width + w-1, pixes + h*width + w);
                        }
                        else{
                            if(collision_count >= size){
                                printf("more allocationg");
                                int new_size = size * 10;
                                size = new_size;
                                printf("resizeing to %d\n", size);
                                colli = (struct collision *)realloc(colli, sizeof(struct collision) * size);
                            }
                            colli[collision_count].left_pix = pixes + h*width + w-1;
                            colli[collision_count].up_pix = pixes + (h-1)*width + w;
                            // copy_pix_value(pixes + h*width + w-1, colli[collision_count].left_pix);
                            // copy_pix_value(pixes + (h-1)*width + w, colli[collision_count].up_pix);
                            copy_pix_value(pixes + h*width + w-1, pixes + h*width +w );
                            handle_collision(colli[collision_count], pixes);
                            collision_count += 1;
                        }
                    }
                    else if(not_background(pixes[(h-1)*width +w])){
                        copy_pix_value(pixes + (h-1)*width + w, pixes + h*width + w);
                    }
                    else if(not_background(pixes[h*width + w-1])){
                        copy_pix_value(pixes + h*width + w-1, pixes + h*width + w);
                    }
                }
                else{
                    if(not_background(pixes[(h-1)*width] )){
                        copy_pix_value(pixes + (h-1)*width, pixes + h*width);
                    }
                }
            }
        }
    }
    printf("coliition cont %d\n", collision_count);
    printf("_______________________________________________\n");
    for(int h=0; h < 6; h++){
        for(int w=0; w < 6; w++){
            printf("%d\t" ,get_total_pixel_value(pixes[h *6 + w]));
        }
        printf("\n");
    }

    // for(int i=0; i< collision_count; i++){
    //     handle_collision(colli+i, pixes);
    // }

   free(colli);
}

int main(){
    srand(time(NULL));
    struct pixel pix[36] = {{0,0,0}, {255,255,255},{0,0,0},{255,255,255}, {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,255,255},{255,255,255},{255,255,255},{255,255,255}, {0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{255,255,255},{0,0,0},{0,0,0},{0,0,0},{255,255,255},{255,255,255},{255,255,255},{0,0,0},{255,255,255},{0,0,0},{0,0,0}}; 

    for(int h=0; h < 6; h++){
        for(int w=0; w < 6; w++){
            printf("%d\t" ,get_total_pixel_value(pix[h *6 + w]));
        }
        printf("\n");
    }
    labeling(pix);
    printf("_______________________________________________\n");
    for(int h=0; h < 6; h++){
        for(int w=0; w < 6; w++){
            printf("%d\t" ,get_total_pixel_value(pix[h *6 + w]));
        }
        printf("\n");
    }
}