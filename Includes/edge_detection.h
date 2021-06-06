#ifndef EDGE_DETETION
#define EDGE_DETETION

#include "../Includes/pixels.h"
#include "../Includes/utils.h"
#include "../Includes/img_operations.h"

void min_max_normalization(int16_t* matrix, int height, int width);
void convolution(int16_t *input, int16_t *output, const int height, const int width, const float *kernel, const int kernel_size, int normalize);
void gussian_filter(int16_t *input, int16_t *output, const int height, const int width, const float sigma);
int16_t *canny_edge_detection(int16_t *input, const int height, const int width, const int tmin, const int tmax);

#endif
