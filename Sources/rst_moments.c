#include <stdio.h>
#include <math.h>

#include "../Includes/rst_moments.h"


double calculate_moment(struct Moments *obj, double power_p, double power_q){
    double moment = 0;
    int height = obj->height;
    int width = obj->width;
    
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if(not_background(obj->pixels_array[h*width+w])){
                if(compare_two_pixels((obj->pixels_array[h*width+w]), obj->search_pixel)){
                    moment += pow(h, power_p) * pow(w, power_q);
                }
            }
        }
    }
    return moment;
}


void calculate_central_row_column(struct Moments *obj){
    obj->central_row = (obj, 1, 0) /  calculate_moment(obj, 0, 0);
    obj->central_column = (obj, 0, 1) /  calculate_moment(obj, 0, 0);
}


double calculate_central_moment(struct Moments *obj, double power_p, double power_q){
    double central_moment = 0;
    int height = obj->height;
    int width = obj->width;

    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if(not_background(obj->pixels_array[h*width+w])){
                if(compare_two_pixels((obj->pixels_array[h*width+w]), obj->search_pixel)){
                    central_moment += pow((h - obj->central_row), power_p) * pow((w - obj->central_column), power_q);
                }
            }
        }
    }
    return central_moment;
}

double calculate_normalized_moment(struct Moments *obj, double power_p, double power_q){
    double tmp = (power_p + power_q) / 2 + 1;
    double normalized_moment = calculate_central_moment(obj, power_p, power_q) / pow(calculate_central_moment(obj, 0, 0), tmp);
    return normalized_moment;
}


void calculate_invariant_moments(struct Moments *obj){
    calculate_central_row_column(obj);

     obj->invariant_moments[0] = calculate_normalized_moment(obj, 2, 0) 

                        + calculate_normalized_moment(obj, 0, 2);


    obj->invariant_moments[1] = pow( (calculate_normalized_moment(obj, 2, 0) - calculate_normalized_moment(obj, 0, 2) ), 2) 

                        + 4 * ( pow(calculate_normalized_moment(obj, 1, 1), 2) );


    obj->invariant_moments[2] = pow( ( calculate_normalized_moment(obj, 3, 0) - 3 * calculate_normalized_moment(obj, 1, 2) ) , 2)

                        + pow( ( 3 * calculate_normalized_moment(obj, 2, 1) - calculate_normalized_moment(obj, 0, 3) ) , 2);


    obj->invariant_moments[3] = pow( (calculate_normalized_moment(obj, 3, 0) + calculate_normalized_moment(obj, 1, 2) ), 2) 

                        + pow( (calculate_normalized_moment(obj, 2, 1) + calculate_normalized_moment(obj, 0, 3) ), 2);


    obj->invariant_moments[4] = ( calculate_normalized_moment(obj, 3, 0) - 3 * calculate_normalized_moment(obj, 1, 2) ) 

                        * ( calculate_normalized_moment(obj, 3, 0) + calculate_normalized_moment(obj, 1, 2) ) 

                        * ( pow(calculate_normalized_moment(obj, 3, 0) + calculate_normalized_moment(obj, 1, 2), 2) 

                        - 3 * pow(calculate_normalized_moment(obj, 2, 1) + calculate_normalized_moment(obj, 0, 3), 2) ) 

                        + ( 3 * calculate_normalized_moment(obj, 2, 1) - calculate_normalized_moment(obj, 0, 3) ) 

                        * ( calculate_normalized_moment(obj, 2, 1) + calculate_normalized_moment(obj, 0, 3) )

                        * ( 3 * pow( calculate_normalized_moment(obj, 3, 0) + calculate_normalized_moment(obj, 1, 2), 2)

                        - pow( calculate_normalized_moment(obj, 2, 1) + calculate_normalized_moment(obj, 0, 3), 2) );

    
    obj->invariant_moments[5] = ( calculate_normalized_moment(obj, 2, 0) - calculate_normalized_moment(obj, 0, 2) ) 

                        * ( pow( calculate_normalized_moment(obj, 3, 0) + calculate_normalized_moment(obj, 1, 2), 2) 

                        - pow( calculate_normalized_moment(obj, 2, 1) + calculate_normalized_moment(obj, 0, 3), 2) )

                        + ( 4 * calculate_normalized_moment(obj, 1, 1) 

                        * ( calculate_normalized_moment(obj, 3, 0) + calculate_normalized_moment(obj, 1, 2) ) 

                        * ( calculate_normalized_moment(obj, 2, 1) + calculate_normalized_moment(obj, 0, 3) ) );


    obj->invariant_moments[6] = ( 3 * calculate_normalized_moment(obj, 2, 1) - calculate_normalized_moment(obj, 0, 3) ) 

                        * ( calculate_normalized_moment(obj, 3, 0) + calculate_normalized_moment(obj, 1, 2) ) 

                        * ( pow( calculate_normalized_moment(obj, 3, 0) + calculate_normalized_moment(obj, 1, 2), 2)

                        - 3 * pow( calculate_normalized_moment(obj, 2, 1) + calculate_normalized_moment(obj, 0, 3), 2) ) 

                        - ( calculate_normalized_moment(obj, 3, 0) - 3 * calculate_normalized_moment(obj, 1, 2) ) 

                        * ( calculate_normalized_moment(obj, 2, 1) + calculate_normalized_moment(obj, 0, 3) ) 

                        * ( 3 * pow( calculate_normalized_moment(obj, 3, 0) + calculate_normalized_moment(obj, 1, 2), 2) 

                        - pow( calculate_normalized_moment(obj, 2, 1) + calculate_normalized_moment(obj, 0, 3), 2) );
}


double *get_finall_moments(struct Moments *obj){
    return obj->invariant_moments;
}

void print_final_moments(struct Moments *obj){
    for(int item=0; item < 7; item ++){
        printf("\tPHI%d: %lf\n",item+1, obj->invariant_moments[item]);
    }
}