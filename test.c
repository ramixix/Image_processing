#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <math.h>
#include <assert.h>
#include <float.h>

#define Max_PI 3.14159265358979323846264338327
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


void set_pixel_value(struct pixel *pix, uint8_t red, uint8_t green, uint8_t blue){
    pix->red = red;
    pix->green = green;
    pix->blue = blue;
}

// void Discrete_Convolution(int *original, int height, int width, int *kernel, int kernel_height, int kernel_width, int padding, int stride){
//     int output_height = ( (height - kernel_height + 2*padding) / stride ) + 1;
//     int output_width = ( (width - kernel_width + 2*padding) / stride ) + 1;
//     int *output = (int *)malloc(sizeof(int) * output_height * output_width);
//     printf("output height: %d , output width: %d\n", output_height, output_width);

//     for(int out_h=0; out_h < output_height; out_h++){
//         for(int out_w=0; out_w < output_width; out_w++){
//             int sum = 0;
//             for(int kernel_h=0; kernel_h < kernel_height; kernel_h++){
//                 for(int kernel_w=0; kernel_w < kernel_width; kernel_w++){
//                     sum += original[ (out_h+kernel_h)*width + (out_w+kernel_w)] * kernel[kernel_h * kernel_width + kernel_w];
//                 }
//             }
//             output[out_h * output_width + out_w]= sum;
//         }
//     }

//     for( int h=0 ; h<output_height; h++){
//         for(int w=0; w<output_width; w++){
//             printf("%d\t", output[h*output_width+w]);
//         }
//         printf("\n");
//     }

//     free(output);
// }

void convolution2(struct pixel *input, uint16_t *output, const int height, const int width, const float *kernel, const int kernel_size, int normalize){
    if(kernel_size % 2 != 1){
        printf("edge_detection.c: convolotion: kernel_size 0/0 2 != 1 false");
        exit(3);
    }

    if( height < kernel_size || width < kernel_size ){
        printf("edge_detection.c: convolution: hieght or width less than kernel_size");
        exit(3);
    }

    int kernel_center = (int)(kernel_size / 2);
    float min = FLT_MAX, max = -FLT_MAX;
 
    if (normalize)
        for (int m = kernel_center; m < height - kernel_center; m++)
            for (int n = kernel_center; n < width - kernel_center; n++) {
                float pixel = 0.0;
                size_t c = 0;
                for (int j = -kernel_center; j <= kernel_center; j++)
                    for (int i = -kernel_center; i <= kernel_center; i++) {
                        pixel += input[(m+j) * width + n+i].red * kernel[c];
                        c++;
                    }
                if (pixel < min)
                    min = pixel;
                if (pixel > max)
                    max = pixel;
                }

    for(int h= kernel_center ; h < height - kernel_center; h++){
        for(int w= kernel_center; w < width - kernel_center; w++){
            float sum = 0.0;
            size_t index = 0;
            for(int kh=-kernel_center; kh <= kernel_center; kh++){
                for(int kw=-kernel_center; kw <= kernel_center; kw++){
                    sum +=  input[(h+kh)*width+(w+kw)].red * kernel[index];
                    index++;
                }
            }
            if (normalize){
                sum = 255 * (sum - min) / (max - min);
                printf("sum _:%d \n", sum);
            }
            output[ h*width + w ] = sum;

        }
    } 
}

// void gussian_filter(int *original, int height, int width, int padding, int stride, const float sigma){
//     const int kernel_size = 2 * (int)(2*sigma) + 3;
//     const float mean = (float)floor(kernel_size/2.0);
//     float kernel[kernel_size * kernel_size];

//     printf("gussian filter kernel size: %d\n" , kernel_size);
//     for(int h=0; h < kernel_size; h++){
//         for(int w=0; w< kernel_size; w++){
//             kernel[h*kernel_size + w] = exp(-0.5 * (pow((h - mean) / sigma, 2.0) + pow((w - mean) / sigma, 2.0)) ) / (2 * Max_PI * sigma * sigma);
//         }
//     }

//     for(int h=0; h < kernel_size; h++){
//         for(int w=0; w< kernel_size; w++){
//             printf("%f ", kernel[h*kernel_size+w]);
//         }
//         printf("\n");
//     }

// }


void gussian_filter2(struct pixel *input, uint16_t *output, const int height, const int width, const float sigma){
    const int kernel_size = 2 * (int)(2*sigma) + 3;
    const float mean = (float)floor(kernel_size/2.0);
    float kernel[kernel_size * kernel_size];
    int sum =0;


    printf("gussian filter kernel size: %d\n" , kernel_size);
    for(int h=0; h < kernel_size; h++){
        for(int w=0; w< kernel_size; w++){
            double x = h - mean;
            double y = w - mean;
            kernel[h*kernel_size + w] = exp(-0.5 * ( pow(x,2) + pow(y, 2) ) * pow(sigma, 2) ) / (2 * Max_PI * sigma * sigma);
        }
    }

    for(int h=0; h < kernel_size; h++){
        for(int w=0; w< kernel_size; w++){
            printf("%f\t", kernel[h*kernel_size+w] );
        }
        printf("\n");
    }

    convolution2(input, output, height, width, kernel, kernel_size, 1);
}


int main(){
    srand(time(NULL));
    
  
    // double max = __DBL_MAX__;
    // printf("%lf", max);


    // float kernel[9] = {0,1,2,2,2,0,0,1,2};
    // struct pixel input[25] = { {255,0,0}, {3,0,0}, {2,0,0}, {1,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {1,0,0}, {3,0,0}, {1,0,0}, {3,0,0}, {1,0,0},
    //                             {2,0,0}, {2,0,0}, {3,0,0}, {2,0,0}, {0,0,0}, {0,0,0}, {2,0,0}, {2,0,0}, {2,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {1,0,0} };
    
    
    // int he = 5, wi=5;
    // uint16_t *output = (uint16_t *)calloc(he*wi*sizeof(uint16_t),1);

    // gussian_filter2(input, output, he, wi, 0.4);
    // // convolution2(input, output, he, wi, kernel, 3);


    // for(int h=0; h < he; h++){
    //     for(int w=0; w < wi; w++){
    //         uint16_t value = output[h*wi+w];
    //         set_pixel_value(input+h*wi+w, value, value, value);
    //     }
    // }

    // for(int h=0; h < he; h++){
    //     for(int w=0; w< wi; w++){
    //         printf("%d\t", input[h*wi+w].blue );
    //     }
    //     printf("\n");
    // }








    double x, y, ret, val, ata;

    x = 5.0;
    y = -12.0;
    val = 180.0 / Max_PI;

    ret = atan2 (y,x) * val;
    ata = atan2(y,x);
    printf("The arc tangent of x = %lf, y = %lf ", x, y);
    printf("is %lf degrees\n", ret);

    printf("\n\n");
    float dir = (float)(fmod(atan2(y, x) + Max_PI, Max_PI) / Max_PI) * 8;
    printf("direction :%lf, atan: %lf", dir, ata);
  
    

}
