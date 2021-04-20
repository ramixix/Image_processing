#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>


#include "../Includes/img_operations.h"
#include "../Includes/pixels.h"
#include "../Includes/utils.h"


int Reset_changes(char *path, struct Image* img){
    FILE* bmp_file = fopen(path, "rb");
    if (path == NULL){
        error("Unable to open file for Reseting file (inside Reset_changes function).\n");
        return -1;
    }
    uint64_t size_to_read = img->width * img->height;
    fseek(bmp_file, img->headersize, SEEK_SET);
    int readed = fread(img->pixels, sizeof(struct pixel), size_to_read, bmp_file);
    if( readed != size_to_read){
        error("Uable to read the old image data to reset changes.( Reset_chagnes fucntion\n");
        printf("%d was suppose to get read but we get %d instead.\n", size_to_read, readed);
        fclose(bmp_file);
        return -1;
    }
    // fclose the image file
    fclose(bmp_file);
    return 0;
}


int Read_bmp(char* bmp_path, struct Image* img){
    // set headersize to standart 54 ( header[14] + headerinfo[40] = 54 )
    img->headersize = Standart_Headersize;

    // opening bmp file
    FILE* bmp_file = fopen(bmp_path, "rb");
    if (bmp_file == NULL){
        error("While opening file ('Read_bmp' function).\n");
        return -1;
    }

    // Reading the DataOffset that determine general header (offset from beginning of file to the beginning of the bitmap data )
    // It has 4 byte (integer size)
    fseek(bmp_file, DataOffset, SEEK_SET);
    int genral_header_size = fread(&img->headersize, sizeof(int), 1, bmp_file);
    if(genral_header_size != 1 ){
        error("Unable to read the general header size ('Read_bmp' function).\n");
        fclose(bmp_file);
        return -1;
    }

    // After finding the dataoffset bring cursor to start of file for futher readings, and then allocate approperiate size for header
    fseek(bmp_file, 0, SEEK_SET);
    img->header = (unsigned char *)malloc(img->headersize);
    if( img->header == NULL){
        error("Unable to allocate memory for header('Read_bmp' function).\n");
        fclose(bmp_file);
        return -1;
    }

    // reading header of bmp file
    int read_bmp = fread(img->header, sizeof(uint8_t), img->headersize, bmp_file);
    if (read_bmp != img->headersize){
        error("While reading header ('Read_bmp' function).\n");
        fclose(bmp_file);
        return -1;
    }

    // First 2 bytes of the BMP file format are the character "B" then the character "M" in ASCII encoding.
    // This block of bytes are at the start of the file and are used to identify the file
    char signature[3];
    snprintf(signature, 3, "%s", img->header);
    if ( strcmp(signature, "BM") != 0 ){
        error("File does not have BM as signature ('Read_bmp' function).\n");
        fclose(bmp_file);
        return -1;
    }

    // check if color bits is equal to 24(8*rgb=24)
    uint16_t bits_per_pixel = *(uint16_t *)&img->header[28];
    if (bits_per_pixel != Bits_Per_Pixel){
        error("Bits per pixel is not 24. Image must be a colored image.('Read_bmp' function).\\n");
        fclose(bmp_file);
        return -1;
    }
    
    // make sure that image is not compressed
    uint32_t isCompressed = *(uint32_t *)&img->header[30];
    if(isCompressed != NoCompression){
        error("Data is not be uncompressed.('Read_bmp' function).\\n");
        fclose(bmp_file);
        return -1;
    }
    
    // After all the checking, extract information from the header
    img->image_size = *(uint32_t *)&img->header[2];
    uint32_t total_size = img->image_size - img->headersize;
    img->width = *(uint32_t *)&img->header[18];
    img->height = *(uint32_t *)&img->header[22];
    uint32_t size_to_read = img->width * img->height;
    img->is_gray = 0;

    printf("\n---------------------------- Image Information --------------------------------\n");
    printf("\t\t* Size of Image: %d bytes\n", img->image_size);
    printf("\t\t* Image Width: %d\n", img->width);
    printf("\t\t* Image Height: %d\n", img->height);
    printf("\t\t* start of array: %d\n", img->headersize);
    printf("\t\t* size of pixel data: %d\n", total_size);
    printf("-------------------------------------------------------------------------------\n");

    // allocate memory for read image pixel data.
    img->pixels = (struct pixel *)malloc(size_to_read*sizeof(struct pixel));
    if( img->pixels == NULL){
        error("Unable to allocate memory for storing pixels ('Read_bmp' function).\n");
        fclose(bmp_file);
        return -1;
    }
    
    // put cursor to starting of the image pixel data then start reading pixel data
    fseek(bmp_file, img->headersize, SEEK_SET);
    int readed = fread(img->pixels, sizeof(struct pixel), size_to_read, bmp_file);
    if( readed != size_to_read){
        error("While reading the image data ('Read_bmp' function).\n");
        printf("%d amount were suppose to get readed but instead it readed %d.\n", size_to_read, readed);
        fclose(bmp_file);
        return -1;
    }

    // fclose the image file
    fclose(bmp_file);
    return 1;
}


void Write_bmp(char *path_to_write, struct Image img){
    int write_to = open(path_to_write, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
    if (write_to == -1){
        error("Unable write file to specified location ('Write_bmp' fucntion).\n");
        exit(-1);
    }
    
    // Write header
    int element_wrote = write(write_to, img.header, img.headersize);
    if( element_wrote != img.headersize ){
        error("Unable to write the header properly('Write_bmp' fucntion).\n");
        close(write_to);
        exit(-1);
    }

    // Write image data
    int size_to_write = img.width * img.height * sizeof(struct pixel);
    int element_wrote1 = write(write_to, img.pixels, size_to_write);
    if( element_wrote1 != size_to_write ){
        error("Unable to write the image pixel data properly('Write_bmp' fucntion).\n");
        close(write_to);
        exit(-1);
    }
    close(write_to);
    printf("\n[+] Image Has Been Saved Successfully.\n");
}


void convert_to_gray(struct Image *img){
    if(img->width == 0 || img->height == 0){
        error("Width and height values are zero.('Conver_to_gray' function)\n");
        exit(-1);
    }
    
    uint8_t byte_per_pixel = 3;
    uint32_t padding  = (4 - (img->width*byte_per_pixel) % 4) % 4;

    printf("\n[+] padding : %d\n", padding);
    for(int h=0; h < img->height; h++){
        for(int w=0; w < img->width; w++){
            rgb_to_gray(img->pixels + h*img->width + w);
        }
        if(padding != 0){
            int i = 0;
            while(padding !=0 ){
                rgb_to_gray(img->pixels + h * img->width + img->width + i);
                i++;
                padding -=1;
            }
        }
    }
    img->is_gray = 1;
    printf("*******************************************************************************\n");
    printf("\t\t\t Image Converted To Gray successfully\t\t\t\t\n");
    printf("*******************************************************************************\n");
   
}


uint32_t *get_histogram(struct Image img){
    uint32_t *histogram_arr;
    int histogram_array_size;
    // If image is grayscal
    if(img.is_gray == 1){
        histogram_array_size = 256;
        histogram_arr = (uint32_t *)malloc(sizeof(uint32_t) * histogram_array_size);
        // set all values of histogram array to 0 before any operation
        memset(histogram_arr, 0, sizeof(uint32_t)*histogram_array_size);
        if( histogram_arr != NULL){
            for(int h=0; h < img.height; h++){
                for(int w=0; w < img.width; w++){
                    // if image is grayscale then all rgb values are the same so take one, we choose green here.
                    uint8_t pixel_value = (img.pixels + h*img.width + w)->red;
                    histogram_arr[pixel_value]++;
                }
            }
        }
    }
    // If image is rgb
    else{
        histogram_array_size = 768; // red + green + blue = 256+256+256
        histogram_arr = (uint32_t *)malloc(sizeof(uint32_t) * histogram_array_size);
        // set all values of histogram array to 0 before any operation
        memset(histogram_arr, 0, sizeof(uint32_t)*histogram_array_size);
        if( histogram_arr != NULL){
            for(int h=0; h < img.height; h++){
                for(int w=0; w < img.width; w++){
                    // when image is rgb then we add the red, green and blue values so total we would have values between 0 < value < 768
                    uint8_t pixel_value = get_total_pixel_value(img.pixels + h*img.width + w);
                    histogram_arr[pixel_value]++;
                }
            }
        }
    }
    return histogram_arr;
}


void convert_to_binary_kmeans(struct Image img){
    double epsilon = 0.000001f;
    double k1=85, k2=170;
    double k3, k4;
    int dividend_k3=0, dividend_k4=0;
    int divisor_k3=0, divisor_k4=0;
    uint16_t iteration = 256;   // assume that image is already grayscale 
    // check if image is grayscale or not. if it is not then ask them if they want to conver to grayscale before continueing.
    if(img.is_gray != 1){
        char answer;
        printf("[?] Do you want grayscale image before convet it to binary?(y/n)");
        scanf("%*c%C", &answer);
        if(answer == 'y'){
            convert_to_gray(&img);
        }
        else{
            // image is not grayscale and user did not want to conver it so we get historgram distribution of total value of rgb pixels
            iteration = 768; // 256+256+256
        }
    }
    uint32_t *histogram_arr = get_histogram(img);
    uint8_t run = 1;
    while(run){
        for(int item=0; item < iteration; item++){
            if( distant(item, k1) < distant(item, k2) ){
                dividend_k3 += item * histogram_arr[item];
                divisor_k3 += histogram_arr[item];
            }else{
                dividend_k4 += item * histogram_arr[item];
                divisor_k4 += histogram_arr[item];
            }
        }
        k3 = (double)dividend_k3 / divisor_k3;
        k4 = (double)dividend_k4 / divisor_k4;
        printf("k1=%lf k2=%lf\n", k1, k2);
        printf("k3=%lf k4=%lf\n", k3, k4);
        if(fabs(k1-k3) < epsilon && fabs(k2-k4) < epsilon ){
            run = 0;
        }
        else{
            k1 = k3;
            k2 = k4;
            divisor_k3 = 0;
            dividend_k3 = 0;
            divisor_k4 = 0;
            dividend_k4 = 0;
        }
    }
    printf("[+] last k1=%lf k2=%lf", k1, k2);
    uint8_t pixel_value;
    for(int h=0; h < img.height; h++){
        for(int w=0; w < img.width; w++){
            if(iteration == 256){
                pixel_value = get_total_pixel_value(img.pixels + h*img.width + w) / 3 ;
            }
            else{
                pixel_value = get_total_pixel_value(img.pixels + h*img.width + w);
            }
            if( distant(pixel_value, k1) < distant(pixel_value, k2)){
                set_pixel(img.pixels + h*img.width + w, 0, 0, 0);
            }
            else{
                set_pixel(img.pixels + h*img.width + w, 255, 255, 255);
            }
        }
    }
    free(histogram_arr);
}





void labeling(struct Image *img){
    int width = img->width;
    int height = img->height;
    struct pixel *pixes = img->pixels;

    for(int h=0; h< height; h++){
        for(int w=0; w < width ; w++){
            if( get_total_pixel_value(pixes + h*width + w) != 0){
                if( w==0 ){
                    set_random_rgb(pixes + h*width );
                }
                else if( get_total_pixel_value(pixes + h*width + w-1) != 0){
                    copy_pix_value(pixes + h*width + w-1, pixes + h*width + w);
                }
                else{
                    set_random_rgb(pixes + h*width + w );
                }
            }
        }
    }

    printf("8888\n");
    for(int h=1; h < height; h++){
        for(int w=0; w < width; w++){
            if( get_total_pixel_value(pixes + h*width + w) != 0 ){
                if( w != 0){
                    if(get_total_pixel_value(pixes + (h-1)*width +w) != 0 && get_total_pixel_value(pixes + h*width + w-1) != 0){
                        if( check_pixs_value(pixes + (h-1)*width + w, pixes + h*width + w-1) ){
                            copy_pix_value(pixes + h*width + w-1, pixes + h*width + w);
                        }
                        else{
                            set_pixel(pixes + h*width + w, 255, 255, 255);
                        }
                    }
                    else if(get_total_pixel_value(pixes + (h-1)*width +w) != 0){
                        copy_pix_value(pixes + (h-1)*width + w, pixes + h*width + w);
                    }
                    else if(get_total_pixel_value(pixes + h*width + w-1) != 0){
                        copy_pix_value(pixes + h*width + w-1, pixes + h*width + w);
                    }
                }
                else{
                    if(get_total_pixel_value(pixes + (h-1)*width ) != 0){
                        copy_pix_value(pixes + (h-1)*width, pixes + h*width);
                    }
                }
            }
        }
    }

   
}
