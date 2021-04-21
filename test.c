#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>



struct pixel
{
    uint8_t red;
    uint8_t green;
    uint8_t blue; 
};

uint16_t get_total_pixel_value(struct pixel *pix){
    return (pix->red + pix->green + pix->blue);
}

void *set_random_rgb(void *pix, int i ){
    struct pixel *pixel = (struct pixel *)pix;
    
    pixel->red =((rand()% 255 )+ i) % 255;

    pixel->green =((rand()% 255 )+ i) % 255;

    pixel->blue =((rand()% 255 )+ i) % 255;
}

int main(){
    // srand(time(NULL));
    // struct pixel pix[9];
    // for(int i=0; i < 9; i++){
    //     set_random_rgb(pix + i, i);
    // }
  
    // // for(int i=0; i < 9; i++){
    // //     pthread_join(t1[i], NULL);
    // //     // set_random_rgb(pix + i);
    // // }

    //    for(int h=0; h < 3; h++){
    //     for(int w=0; w < 3; w++){
    //         printf("r:%d g:%d b:%d\t" ,(pix + h *3 + w)->red,(pix + h *3 + w)->green,(pix + h *3 + w)->blue);
    //     }
    //     printf("\n");
    // }

    uint8_t a = 255, b=4, c=254, g=0;
 
    if(a==4){
        if(c==4){
            printf("a=2 ,c=3");
        }else{
             printf("a=2 , c!=3");
        }
       
    }
    else if(0==0){
        printf("a=%d, b=%d, c=%d, g=%d", a,b,c,g);
    }
    
}