#ifndef RST_MOMENTS
#define RST_MOMNETS

#include "pixels.h"

struct Moments{
    double invariant_moments[7];
    double central_column, central_row;
    int height, width;
    struct pixel *pixels_array;
    struct pixel search_pixel;
};

double calculate_moment(struct Moments *obj, double power_p, double power_q);
void calculate_central_row_column(struct Moments *obj);
double calculate_central_moment(struct Moments *obj, double power_p, double power_q);
double calculate_normalized_moment(struct Moments *obj, double power_p, double power_q);
void calculate_invariant_moments(struct Moments *obj);
double *get_finall_moments(struct Moments *obj);
void print_final_moments(struct Moments *obj);

#endif