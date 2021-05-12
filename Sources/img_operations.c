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
#include "../Includes/linked_list.h"
#include "../Includes/rst_moments.h"
#include "../Includes/morphology.h"


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
    int width = img->width;
    int height = img->height;
    printf("\n[+] padding : %d\n", padding);
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            rgb_to_gray(img->pixels + h*width + w);
        }
        if(padding != 0){
            int i = 0;
            while(padding !=0 ){
                rgb_to_gray(img->pixels + h*width + width+i);
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
    uint32_t *obj_array;
    int obj_arrayay_size;
    // If image is grayscal
    if(img.is_gray == 1){
        obj_arrayay_size = 256;
        obj_array = (uint32_t *)malloc(sizeof(uint32_t) * obj_arrayay_size);
        // set all values of histogram array to 0 before any operation
        memset(obj_array, 0, sizeof(uint32_t)*obj_arrayay_size);
        if( obj_array != NULL){
            for(int h=0; h < img.height; h++){
                for(int w=0; w < img.width; w++){
                    // if image is grayscale then all rgb values are the same so take one, we choose green here.
                    uint8_t pixel_value = (img.pixels + h*img.width + w)->red;
                    obj_array[pixel_value]++;
                }
            }
        }
    }
    // If image is rgb
    else{
        obj_arrayay_size = 768; // red + green + blue = 256+256+256
        obj_array = (uint32_t *)malloc(sizeof(uint32_t) * obj_arrayay_size);
        // set all values of histogram array to 0 before any operation
        memset(obj_array, 0, sizeof(uint32_t)*obj_arrayay_size);
        if( obj_array != NULL){
            for(int h=0; h < img.height; h++){
                for(int w=0; w < img.width; w++){
                    // when image is rgb then we add the red, green and blue values so total we would have values between 0 < value < 768
                    uint8_t pixel_value = get_total_pixel_value(img.pixels[h*img.width + w]);
                    obj_array[pixel_value]++;
                }
            }
        }
    }
    return obj_array;
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
    uint32_t *obj_array = get_histogram(img);
    uint8_t run = 1;
    while(run){
        for(int item=0; item < iteration; item++){
            if( distant(item, k1) < distant(item, k2) ){
                dividend_k3 += item * obj_array[item];
                divisor_k3 += obj_array[item];
            }else{
                dividend_k4 += item * obj_array[item];
                divisor_k4 += obj_array[item];
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
    uint8_t backgournd = 0;
    uint8_t foreground = 255;
    uint8_t choise;
    printf("\nSelect an option (1) or (2)? (don't worry it's not a big deal): ");
    scanf("%d", &choise);
    while( (getchar()) != '\n');

    for(int h=0; h < img.height; h++){
        for(int w=0; w < img.width; w++){
            if(iteration == 256){
                pixel_value = img.pixels[h*img.width + w].red ;
            }
            else{
                pixel_value = get_total_pixel_value(img.pixels[h*img.width + w]);
            }

            if(choise == 1){
                if( distant(pixel_value, k1) < distant(pixel_value, k2)){
                    set_pixel_value(img.pixels + h*img.width + w, foreground, foreground, foreground);
                }
                else{
                    set_pixel_value(img.pixels + h*img.width + w, backgournd, backgournd, backgournd);
                }
            }
            else{
                if( distant(pixel_value, k1) < distant(pixel_value, k2)){
                    set_pixel_value(img.pixels + h*img.width + w, backgournd, backgournd, backgournd);
                }
                else{
                    set_pixel_value(img.pixels + h*img.width + w, foreground, foreground, foreground);
                }
            }
        }
    }
    free(obj_array);
    printf("\n\nImage Converted To Binary Successfully.\n");
}


void morphology(struct Image *img){
    int foreground = 1;
    int structing_image[] = {foreground, foreground, foreground, foreground, foreground, foreground, foreground, foreground, foreground};
    dilation(img->pixels, structing_image, img->height, img->width, 3, 3);
    // erision(img->pixels, structing_image, img->height, img->width, 3, 3);
}


void handel_collision(struct collision collided, struct pixel *pixes, int height, int width){
    struct pixel left, up;
    copy_pixel_value(&left, collided.left_pix);
    copy_pixel_value(&up, collided.up_pix);
    uint16_t left_value = get_total_pixel_value(left);
    uint16_t up_value = get_total_pixel_value(up);
    struct pixel throw;
    struct pixel substitute;
    if(left_value > up_value){
        copy_pixel_value(&throw, &left);
        copy_pixel_value(&substitute, &up);
    }
    copy_pixel_value(&throw, &up);
    copy_pixel_value(&substitute, &left);

    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if(compare_two_pixels(pixes[h*width + w], throw)){
                copy_pixel_value(pixes + h*width + w, &substitute);
            }
        }
    }
}



void labeling(struct Image *img){
    int width = img->width;
    int height = img->height;
    struct pixel *pixes = img->pixels;
    int collision_count = 0;
    int collision_arr_size = 100;
    struct collision *collisions = (struct collision *)malloc(sizeof(struct collision) * collision_arr_size); // initial size is 10, if there be more than 10 collision then we will reallocate more memory later.
    for(int h=0; h< height; h++){
        for(int w=0; w < width ; w++){
            if(not_background(pixes[h*width + w])){
                if( w==0 ){
                    set_random_rgb(pixes + h*width );
                }
                else if(not_background(pixes[h*width + w-1])){
                    copy_pixel_value(pixes + h*width + w, pixes + h*width + w-1);
                }
                else{
                    set_random_rgb(pixes + h*width + w );
                }
            }
        }
    }
    for(int h=1; h < height; h++){
        for(int w=0; w < width; w++){
            if(not_background(pixes[h*width + w])){
                if( w != 0){
                    if(not_background(pixes[(h-1)*width +w]) && not_background(pixes[h*width + w-1]) ){
                        if(compare_two_pixels(pixes[(h-1)*width + w], pixes[h*width + w-1])){
                            copy_pixel_value(pixes + h*width + w , pixes + (h-1)*width + w);
                        }
                        else{
                            // check if the memory allocated for collision is enogh
                            if(collision_count >= collision_arr_size){
                                printf("Allocating more memory for collisions array...\n");
                                int tmp_size = collision_arr_size * 8; // allocate 8 times more momery 
                                collision_arr_size = tmp_size;
                                collisions = (struct collision*)realloc(collisions, sizeof(struct collision) * collision_arr_size);
                            }
                            // specifying the left and up pixel of the pixel that collision occure
                            collisions[collision_count].left_pix = pixes + h*width + w-1;
                            collisions[collision_count].up_pix = pixes + (h-1)*width + w;
                            // set the collided pixel value to one pixel left
                            copy_pixel_value(pixes + h*width + w, pixes + h*width + w-1);
                            handel_collision(collisions[collision_count] , pixes, height, width);
                            collision_count += 1;
                        }
                    }
                    else if(not_background(pixes[(h-1)*width + w])){
                        copy_pixel_value(pixes + h*width + w , pixes + (h-1)*width + w);
                    }
                    else if(not_background(pixes[h*width + w-1])){
                        copy_pixel_value(pixes + h*width + w , pixes + h*width + w-1);
                    }
                }
                else{
                    if(not_background( pixes[(h-1)*width])){
                        copy_pixel_value(pixes + h*width , pixes + (h-1)*width);
                    }
                }
            }
        }
    }

    printf("collision count %d\n", collision_count);
    // for(int item=0; item < collision_count; item++){
    //     handel_collision(collisions[item] , pixes, height, width);
    // }
    free(collisions);
    printf("Image Has Been Labled Successfully.\n");
}


void draw_bounding_box(struct pixel *pixels, int height, int width, int min_column, int max_column, int min_row, int max_row){
    struct pixel white_bound = { 255, 255, 255};
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if( w >= min_column && w <= max_column && h >= min_row && h <= max_row){
                if( w == min_column || w == max_column){
                    copy_pixel_value(pixels + h*width + w, &white_bound);
                }
                if( h == min_row || h == max_row){
                    copy_pixel_value(pixels + h*width + w, &white_bound);
                }
            }
        }
    }
}

int find_min_column(struct pixel *pixels, int height, int width, struct pixel search_for_pixel){
    int min_column = INT32_MAX;
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if( compare_two_pixels( pixels[h*width+w], search_for_pixel) ){
                if( w < min_column){
                    min_column = w;
                } 
            }
        }
    }
    return min_column;
}

int find_max_column(struct pixel *pixels, int height, int width, struct pixel search_for_pixel){
    int max_column = 0;
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if( compare_two_pixels( pixels[h*width+w], search_for_pixel) ){
                if( w > max_column){
                    max_column = w;
                } 
            }
        }
    }
    return max_column;
}

int find_min_row(struct pixel *pixels, int height, int width, struct pixel search_for_pixel){
    int min_row = INT32_MAX;
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if( compare_two_pixels( pixels[h*width+w], search_for_pixel) ){
                if( h < min_row){
                    min_row = h;
                } 
            }
        }
    }
    return min_row;
}

int find_max_row(struct pixel *pixels, int height, int width, struct pixel search_for_pixel){
    int max_row = 0;
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if( compare_two_pixels( pixels[h*width+w], search_for_pixel) ){
                if( h > max_row){
                    max_row = h;
                } 
            }
        }
    }
    return max_row;
}


void find_poisition_for_bounding_box(struct pixel *pixels, int height, int width, struct pixel search_for_pixel){
    int min_column = find_min_column(pixels, height, width, search_for_pixel) - 2; // give 2 pixel space for every part of bounding box
    int max_column = find_max_column(pixels, height, width, search_for_pixel) + 2;
    int min_row = find_min_row(pixels, height, width, search_for_pixel) - 2;
    int max_row = find_max_row(pixels, height, width, search_for_pixel) + 2;
    draw_bounding_box(pixels, height, width, min_column, max_column, min_row, max_row);
}


void bounding_box(struct Image *img){
    int height = img->height;
    int width = img->width;
    struct pixel bounding_box_color = {255, 255, 255}; // bounding box color = white, we don't want to bouding box around another bounding box if user want to draw bounding box on image that is already bounded
    struct pixel_node *head = NULL;
    extern int Number_of_Nodes;
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if(not_background(img->pixels[h*width+w]) && compare_two_pixels(img->pixels[h*width+w], bounding_box_color)==0 ){
                add_if_not_exists(&head, img->pixels[h*width+w]);
            }
        }
    }
    printf("%d Objects found inside image\n", Number_of_Nodes);
    printf("It might take some time to draw bouding box. Please be patient.\n");
    for(int i=0; i < Number_of_Nodes ; i++){
        struct pixel *search_for_pixel = get_node_pixel(head, i);
        find_poisition_for_bounding_box(img->pixels, height, width, *search_for_pixel);
    }
   
    free_all(&head);
    printf("Boudning Box Has Been Drawn Successfully.\n");
}


void feature_extraction(struct Image *img){
    int height = img->height;
    int width = img->width;
    struct pixel bounding_box_color = {255, 255, 255}; // we don't want to extract feature for bounding boxes.
    struct pixel_node *head = NULL;
    extern int Number_of_Nodes;
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if(not_background(img->pixels[h*width+w]) && compare_two_pixels(img->pixels[h*width+w], bounding_box_color)==0 ){
                add_if_not_exists(&head, img->pixels[h*width+w]);
            }
        }
    }
    printf("%d Objects found inside image\n", Number_of_Nodes);
    printf("Featrue extraction might take some time to draw bouding box. Please be patient.\n");
    struct Moments objs_moment[Number_of_Nodes];

    for(int i=0; i < Number_of_Nodes ; i++){
        struct pixel *search_for_pixel = get_node_pixel(head, i);
        objs_moment[i].height = height;
        objs_moment[i].width = width;
        copy_pixel_value(&(objs_moment[i].search_pixel), search_for_pixel);
        objs_moment[i].pixels_array = img->pixels;
        calculate_invariant_moments(objs_moment+i);
        printf("\n%d. object moments vlaue:\n", i);
        print_final_moments(objs_moment+i);
        printf("Average invariant moments: %lf\n", average_invariant_moment(objs_moment+i));
        printf("--------------------------------------------------------------------");
    }
   
    free_all(&head);
}

