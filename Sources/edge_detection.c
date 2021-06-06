#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "../Includes/edge_detection.h"

#define PI 3.14159265358979323846264338327
#define ForeGround_Color 255
#define BackGround_Color 0


void min_max_normalization(int16_t* matrix, int height, int width) {
	int min = 1000000, max = 0, i, j;
	
	for(i = 0; i < height; i++) {
		for(j = 0; j < width; j++) {
			if (matrix[i*width+j] < min) {
				min = matrix[i*width+j];
			}
			else if (matrix[i*width+j] > max) {
				max = matrix[i*width+j];
			}
		}
	}
	
	for(i = 0; i < height; i++) {
		for(j = 0; j < width; j++) {
			double ratio = (double) (matrix[i*width+j] - min) / (max - min);
			matrix[i*width+j]  = ratio * ForeGround_Color;
		}
	} 
}


void convolution(int16_t *input, int16_t *output, const int height, const int width, const float *kernel, const int kernel_size, int normalize){
    if(kernel_size % 2 != 1){
        error("edge_detection.c: convolotion: kernel_size 0/0 2 != 1 false");
        exit(3);
    }

    if( height < kernel_size || width < kernel_size ){
        error("edge_detection.c: convolution: hieght or width less than kernel_size");
        exit(3);
    }

    const int kernel_center = kernel_size / 2;
    float min = FLT_MAX, max = -FLT_MAX;
 
    if (normalize){
        for (int h = kernel_center; h < height - kernel_center; h++){
            for (int w = kernel_center; w < width - kernel_center; w++){
                float sum = 0.0;
                size_t index = 0;
                for (int kh = -kernel_center; kh <= kernel_center; kh++){
                    for (int kw = -kernel_center; kw <= kernel_center; kw++) {
                        sum +=  input[(h+kh)*width+(w+kw)] * kernel[index];
                        index++;
                    }
                }
                if (sum < min)
                    min = sum;
                if (sum > max)
                    max = sum;
            }
        }
    }
    for(int h= kernel_center ; h < height - kernel_center; h++){
        for(int w= kernel_center; w < width - kernel_center; w++){
            float sum = 0.0;
            size_t index = 0;
            for(int kh=-kernel_center; kh <= kernel_center; kh++){
                for(int kw=-kernel_center; kw <= kernel_center; kw++){
                    sum +=  input[(h+kh)*width+(w+kw)] * kernel[index];
                    index++;
                }
            }
            if(normalize)
                sum = ForeGround_Color * (sum - min) / (max - min);
            output[h*width + w] = sum;

        }
    } 
}


/*
 * gaussianFilter:
 * determine size of kernel (odd #)
 * 0.0 <= sigma < 0.5 : 3
 * 0.5 <= sigma < 1.0 : 5
 * 1.0 <= sigma < 1.5 : 7
 * 1.5 <= sigma < 2.0 : 9
 * 2.0 <= sigma < 2.5 : 11
 * 2.5 <= sigma < 3.0 : 13 ...
 * kernelSize = 2 * int(2*sigma) + 3;
 */
void gussian_filter(int16_t *input, int16_t *output, const int height, const int width, const float sigma){
    const int kernel_size = 2 * (int)(2*sigma) + 3;
    const float mean = (float)floor(kernel_size/2.0);
    float kernel[kernel_size * kernel_size];
    int sum =0;


    printf("gussian filter kernel size: %d\n" , kernel_size);
    for(int h=0; h < kernel_size; h++){
        for(int w=0; w< kernel_size; w++){
            double x = h - mean;
            double y = w - mean;
            kernel[h*kernel_size + w] = exp(-0.5 * ( pow(x,2) + pow(y, 2) ) * pow(sigma, 2) ) / (2 * PI * sigma * sigma);
        }
    }

    convolution(input, output, height, width, kernel, kernel_size, 1);
}



int16_t *canny_edge_detection(int16_t *input, const int height, const int width, const int tmin, const int tmax){

    int16_t *Gradiant =  (int16_t *)calloc(height * width, sizeof(int16_t));
    int16_t *Gx = (int16_t *)calloc(height * width, sizeof(int16_t));
    int16_t *Gy = (int16_t *)calloc(height * width, sizeof(int16_t));
    int16_t *output = (int16_t *)calloc(height * width, sizeof(int16_t));
    int16_t *Non_max_suppression = calloc(height * width, sizeof(int16_t));
 
    if (Gradiant == NULL || Gx == NULL || Gy == NULL || output == NULL) {
        error( "edge_detection.c: canny_edge_detection: Failed memory allocation(s).\n");
        exit(3);
    }

    gussian_filter(input, output, height, width, 0.4);

    const float Gx_derivative[] = { -1, 0, 1,
                                    -2, 0, 2,
                                    -1, 0, 1};
 
    convolution(output, Gx, height, width, Gx_derivative, 3, 0);

    const float Gy_derivative[] = { 1, 2, 1,
                                    0, 0, 0,
                                    -1,-2,-1};
 
    convolution(output, Gy, height, width, Gy_derivative, 3, 0);

    for (int h = 1; h < height - 1; h++){
        for (int w = 1; w < width - 1; w++) {
            const int index = h * width + w;
            Gradiant[index] = (int16_t)hypot(Gx[index], Gy[index]);
        }
    }

    // Compare the edge strength of the current pixel with the edge strength of the pixel in the positive and negative gradient directions.
    //If the edge strength of the current pixel is the largest compared to the other pixels in the mask with the same direction 
    // (e.g., a pixel that is pointing in the y-direction will be compared to the pixel above and below it in the vertical axis), the value will be preserved.
    // Otherwise, the value will be suppressed.
    
    //  ____ ____ ____
    // | ne | nn | nw |
    //  ---- ---- ----
    // | ee | c  | ww |
    //  ---- ---- ----
    // | se | ss | sw |
    //  ---- ---- ----
    
    for (int h = 1; h < height - 1; h++){
        for (int w = 1; w < width - 1; w++) {
            const int c = h * width + w;    //center
            const int nn = c - width;       //north 
            const int ss = c + width;       //south
            const int ww = c + 1;           //west
            const int ee = c - 1;           //east
            const int nw = nn + 1;          //north-west
            const int ne = nn - 1;          //north-east
            const int sw = ss + 1;          //south-west
            const int se = ss - 1;          //south-east
 

            const float direction = (float)(fmod(atan2(Gy[c], Gx[c]) + PI, PI) / PI) * 8;
            //    (                           0 deg                                ) || (                                   45 deg                        ) || (                                   90 deg                        ) || (                               135 deg                            )
            if ( ((direction <= 1 || direction > 7) && Gradiant[c] > Gradiant[ee] && Gradiant[c] > Gradiant[ww]) || ((direction > 1 && direction <= 3) && Gradiant[c] > Gradiant[nw] && Gradiant[c] > Gradiant[se]) || ((direction > 3 && direction <= 5) && Gradiant[c] > Gradiant[nn] && Gradiant[c] > Gradiant[ss]) || ((direction > 5 && direction <= 7) && Gradiant[c] > Gradiant[ne] && Gradiant[c] > Gradiant[sw]))
                Non_max_suppression[c] = Gradiant[c];
            else
                Non_max_suppression[c] = BackGround_Color;
        }
    }

    
    memset(output, 0, height*width*sizeof(int16_t));
    memset(Gx, 0, height*width*sizeof(int16_t));
    int *edges = (int *)Gx;
    
    for(int h=1; h < height-1; h++){
        for(int w=1; w < width-1; w++){
            int position = h*width+w;
            if(Non_max_suppression[position] >= tmax && output[position]==0){
                output[position] = ForeGround_Color;
                int Number_of_edges = 1;
                edges[0] = position;

                while(Number_of_edges > 0){
                    Number_of_edges--;
                    const int edge_position = edges[Number_of_edges];
                    int neighbours[8];
                    neighbours[0] = edge_position - width;  //nn
                    neighbours[1] = edge_position + width;  //ss
                    neighbours[2] = edge_position - 1;      //ee
                    neighbours[3] = edge_position + 1;      //ww
                    neighbours[4] = neighbours[0] - 1;      //ne
                    neighbours[5] = neighbours[0] + 1;      //nw
                    neighbours[6] = neighbours[1] - 1;      //se
                    neighbours[7] = neighbours[1] + 1;      //sw

                    for(int posi=0; posi < 8; posi++){
                        if(Non_max_suppression[neighbours[posi]] >= tmin && output[neighbours[posi]] ==0 ){
                            output[neighbours[posi]] = ForeGround_Color;
                            edges[Number_of_edges] = neighbours[posi];
                            Number_of_edges++;
                        }
                    }

                }
            }
        }
    }

    free(Gx);
    free(Gy);
    free(Gradiant);
    free(Non_max_suppression);
    return output;
}

