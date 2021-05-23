#include <stdio.h>
#include <math.h>
#include "../Includes/utils.h"


// function that is used to pring errors
void error(char *message){
    fprintf(stderr, "[Error] %s", message);
}

int check_null(void *ptr, char *message){
    if(ptr == NULL){
        error(message);
        return -1;
    }
    return 0;
}

// calculate distance between tow number. this function is used while calculating binary image in kmeans algorithm
double distant(double num1, double num2){
    double result = (num1 - num2) * (num1 - num2);
    result = sqrt(result);
    return result;
}