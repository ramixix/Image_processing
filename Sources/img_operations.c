#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <math.h>


// adding include files inside Includes directory
#include "../Includes/img_operations.h"
#include "../Includes/pixels.h"
#include "../Includes/utils.h"
#include "../Includes/linked_list.h"
#include "../Includes/rst_moments.h"
#include "../Includes/morphology.h"
#include "../Includes/data_base.h"
#include "../Includes/edge_detection.h"


/* 
    Reset all changes that is done to image.
    image path is the first thing that user should specify to run the program, what we do is 
    to simply go again to that specified path and read all data back again, therefore we reset 
    all change by replacing all data with the original one.

    Return :
        '-1' if success
        ' 1' if fail
 */
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


/* 
    Read Image for further operations.
    Return :
        '-1' if success
        ' 1' if fail
 */
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
    // This block of bytes are at the start of the file and are used to identify the file (file signutures)
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


/* 
    Save Image to specified path.
    Return :
        '-1' if success
        ' 1' if fail
 */
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


/* 
    Conver Image to gray scale.
    get every pixel mean value by adding red, green and blue values together and then divide the total by 3.
    after that by setting pixel's red, green and blue values all to the same mean we would have the gray version of image.
 */
void convert_to_gray(struct Image *img){
    if(img->width == 0 || img->height == 0){
        error("Width and height values are zero.('Conver_to_gray' function)\n");
        exit(-1);
    }
    // first calculating padding
    uint8_t byte_per_pixel = 3;
    uint32_t padding  = (4 - (img->width*byte_per_pixel)%4) % 4;
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


/* 
    Calculating histogram array for image.
    histogram array is 256 size. index from 0 to 255 so basically has 1 index for every possible value of a pixel's red, green and blue.
    if image is converted to gray then fucntion will loop through every pixel and get one of the red, green, blue values as they are all
    the same when image is gray then it won't differe which one we choose. after getting the value of pixel we add one to that index value,
    and this way we can calculate the number of times a pixel value oocure in image.
    Return :
        unit32_t pointer that points to calculated histogram array
 */
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


/* 
    Binary image (image that only contains the value of 0's and 1's) using KMEANS method.
    by looking at historgram array we calculate two mean values using kmeans algorithm and then loop all over the image data 
    and control which pixel's value is closer to which means and according to this we set pixel's value either to 0(black) or 255(white)

 */
void convert_to_binary_kmeans(struct Image img){
    double epsilon = 0.000001f;
    double k1=75, k2=150;
    double next_k1, next_k2;
    double dividend_near_k1=0, dividend_near_k2=0;
    double divisor_near_k1=0, divisor_near_k2=0;
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
                dividend_near_k1 += item * obj_array[item];
                divisor_near_k1 += obj_array[item];
            }
            else{
                dividend_near_k2 += item * obj_array[item];
                divisor_near_k2 += obj_array[item];
            }
        }
        
        if(divisor_near_k1 == 0 || divisor_near_k2 == 0){
            k1 = fmod(k1+50.0, 256);
            k2 = fmod(k2+50.0, 256);
            continue;
        }
        next_k1 = dividend_near_k1 / divisor_near_k1;
        next_k2 = dividend_near_k2 / divisor_near_k2;
        
    
        printf("k1=%lf k2=%lf\n", k1, k2);
        printf("k3=%lf k4=%lf\n", next_k1, next_k2);
        if(fabs(k1-next_k1) < epsilon && fabs(k2-next_k2) < epsilon ){
            run = 0;
        }
        else{
            k1 = next_k1;
            k2 = next_k2;
            dividend_near_k1 = 0;
            divisor_near_k1 = 0;
            dividend_near_k2 = 0;
            divisor_near_k2 = 0;
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
    
    kinda_region_filling(img->pixels, structing_image, img->height, img->width, 3, 3);
    opening(img->pixels, structing_image, img->height, img->width, 3, 3);
    closing(img->pixels, structing_image, img->height, img->width, 3, 3);
    // boundary_extraction(img->pixels, structing_image, img->height, img->width, 3, 3);
  
    // erision(img->pixels, structing_image, img->height, img->width, 3, 3);
    // dilation(img->pixels, structing_image, img->height, img->width, 3, 3);   
}


/* 
    This function is used and called when label function is called to label image.
    every time a collision occures while labeling an image this fucntion goes through all pixels from the first of image data
    and handle the collision by setting the collision pixels to smaller value.
 */
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


/* 
    Label Image. loop over image data and label each pixel by assining a unique color to pixels that are connected.
    we label image according to '4-neighborhood region identification' method. we made a list of size 100 for collisions that might occur while labeling, 
    beside that we count every time a collision occures and if this number became bigger than the collision array size we reallocate
    memory for collision list by multiplying previous array size to 8. every time that a collision occures we save needed information about it
    in collision struct and right away handle_collision fucntion is called to correct the collision. pixels that have same label
    are basically belong to the same object therefore in the end all objects inside the image have different color.
 */
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


/* 
    Draw bounding box using minimun row, column and maximum row, column that we find inside "find_poisition_for_bounding_box" function.
    for every object, loop over image and according to postions that we calculate for that object draw white line at top, down, left and right
    of object therefore this way we drawed bounding box.
 */
void draw_bounding_box(struct pixel *pixels, int height, int width, int min_column, int max_column, int min_row, int max_row, struct pixel bounding_box_color){
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            // every pixel that is in area of the object
            if( w >= min_column && w <= max_column && h >= min_row && h <= max_row){
                // set pixel color to white if pixel is on the line top or bottom
                if( w == min_column || w == max_column){
                    copy_pixel_value(pixels + h*width + w, &bounding_box_color);
                }
                // set pixel color to white if pixel is on the line left or right
                if( h == min_row || h == max_row){
                    copy_pixel_value(pixels + h*width + w, &bounding_box_color);
                }
            }
        }
    }
}



/* 
    Find the minimum column position of an object.
    Return :
        integer ==> minimum column position
 */
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


/* 
    Find the maximum column position of an object.
    Return :
        integer ==> maximum column position
 */
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


/* 
    Find the minimum row position of an object.
    Return :
        integer ==> minimum row position
 */
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


/* 
    Find the maximum row position of an object.
    Return :
        integer ==> maximum row position
 */
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


/* 
    Determining edge positions of the bounding box that is going to draw around an object.
    after find the 4 edges of the bouding box we basically draw the bouding box.
    2 pixel space is added to every edge to prevent colliding bounding box with object.
 */
void find_poisition_for_bounding_box(struct pixel *pixels, int height, int width, struct pixel search_for_pixel, struct pixel bounding_box_color){
    // give 2 pixel space for every bounding box of each object to prevent colliding bounding box with object itself.
    int min_column = find_min_column(pixels, height, width, search_for_pixel) - 2; 
    int max_column = find_max_column(pixels, height, width, search_for_pixel) + 2;
    int min_row = find_min_row(pixels, height, width, search_for_pixel) - 2;
    int max_row = find_max_row(pixels, height, width, search_for_pixel) + 2;
    draw_bounding_box(pixels, height, width, min_column, max_column, min_row, max_row, bounding_box_color);
}


/* 
    draw bounding box around detected objects inside image.
    first loop over the image data and find the detected object(after labeling image every object is assigned different colores ) 
    then add detected objects to a list that is created using linked list algorhim. loop over every object inside that list and draw bounding box
    for every and each of the objects.

 */
void bounding_box(struct Image *img){
    int height = img->height;
    int width = img->width;
    struct pixel bounding_box_color = {255, 255, 255}; // bounding box color = white, we don't want to bouding box around another bounding box if user want to draw bounding box on image that is already bounded
    struct pixel_node *head = NULL; // initial the single list

    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if(not_background(img->pixels[h*width+w]) && compare_two_pixels(img->pixels[h*width+w], bounding_box_color)==0 ){
                add_if_not_exists(&head, img->pixels[h*width+w]);
            }
        }
    }
    extern int Number_of_Nodes;
    int node_number = Number_of_Nodes;
    printf("%d Objects found inside image\n", node_number);
    printf("It might take some time to draw bouding box. Please be patient.\n");
    for(int i=0; i < node_number ; i++){
        struct pixel *search_for_pixel = get_node_pixel(head, i);
        find_poisition_for_bounding_box(img->pixels, height, width, *search_for_pixel, bounding_box_color);
    }
   
    free_all(&head);
    printf("Boudning Box Has Been Drawn Successfully.\n");
}



/* 
    Calculate mean values for every object detected and ask users if they want to save means and objects to
    database

 */
void feature_extraction(struct Image *img, uint8_t matching_check){
    int height = img->height;
    int width = img->width;
    struct pixel bounding_box_color = {255, 255, 255}; // we don't want to extract feature for bounding boxes.
    const uint8_t PHI_Num = 7;
    struct pixel_node *head = NULL;
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            if(not_background(img->pixels[h*width+w]) && compare_two_pixels(img->pixels[h*width+w], bounding_box_color)==0 ){
                add_if_not_exists(&head, img->pixels[h*width+w]);
            }
        }
    }

    extern int Number_of_Nodes;
    int node_number = Number_of_Nodes;
    struct Moments objs_moment[node_number];
    struct Moments *total_objects_moment[node_number];
    double average_moments[node_number];
    printf("%d Objects found inside image\n", node_number);
    printf("Featrue extraction might take some time. Please be patient.\n");

    for(int i=0; i < Number_of_Nodes ; i++){
        struct pixel *search_for_pixel = get_node_pixel(head, i);
        objs_moment[i].height = height;
        objs_moment[i].width = width;
        copy_pixel_value(&(objs_moment[i].search_pixel), search_for_pixel);
        objs_moment[i].pixels_array = img->pixels;
        calculate_invariant_moments(objs_moment+i);
        total_objects_moment[i] = objs_moment + i;
        if(matching_check)
            average_moments[i] = average_invariant_moment(objs_moment+i);
        
        // printf("%d. object moments vlaue:\n", i);
        // print_final_moments(objs_moment+i);
        // printf("Average invariant moments: %lf\n", average_invariant_moment(objs_moment+i));
        // printf("--------------------------------------------------------------------\n");
    }

    if(matching_check == 0){
        double final_PHIS[PHI_Num];
        double average_PHIS=0;
        for(int i=0; i < PHI_Num ; i++){
            for(int j=0; j < node_number; j++){
                final_PHIS[i] += total_objects_moment[j]->invariant_moments[i];
            }
        }
        for(int i=0; i < PHI_Num; i++){
            final_PHIS[i] = final_PHIS[i] / node_number;
            average_PHIS += final_PHIS[i];
            printf("Final PHI %d: %lf\n", i, final_PHIS[i]);
        }
        save_to_databbase(average_PHIS/PHI_Num, node_number);
    }
    else{
        char object_name[256];
        for(int i=0; i < node_number; i++){
            minimum_distance_obj_detection(average_moments[i], object_name);
            printf("\nObject %d is a(n) '%s'\n", i, object_name);
        }
    }
   
    free_all(&head);
}



void save_to_databbase(double average_moments, int node_number){
    int ch;
    printf("\n7 Invariant Moment values and one total mean of these 7 value have been calculated for every and each of the %d objects.\n", node_number);
    printf("Would you like to save the means inside database?(y/n) ");
    scanf("%c", &ch);
    printf("%d", ch);
    while( (getchar()) != '\n');

    if( ch == 121 || ch == 89 ){    // 89 and 121 are equvalent decimal value for 'Y' and 'y' characters.
        // int choise;
        // printf("How Do You Want To Save This Valuse?\n");
        // printf("1 - Save All Automatically Under a General Name.(all values are going to be saved to database under same name but different ID's)\n");
        // printf("2 - Explicitly Specify Names For Every Object\n");
        // scanf("%d", &choise);
        // while( (getchar()) != '\n');

        // if(choise == 1){
        char name[50];
        printf("\nEnter The General Name For Objects: ");
        scanf("%50[^\n]", name);
        while( (getchar()) != '\n');
        int starting_id = find_biggest_id();
        printf("%d\n", starting_id);
        // for(int i=0; i<node_number; i++){
        append_auto(starting_id , name, average_moments);
        // }
        // }
        // if(choise == 2){
        //     int starting_id = find_biggest_id();
        //     for(int i=0; i<node_number; i++){
        //         char name[50];
        //         printf("Enter Name For %d Object: ",i);
        //         scanf("%50[^\n]", name);
        //         while( (getchar()) != '\n');
        //         append_auto(starting_id+i, name, average_moments[i]);
        //     }
        // }
    }
}


void check_matching(struct Image *img){
    uint8_t mathcing_check = 1;
    convert_to_gray(img);
    fprintf(stdin, "%d" , 1);
    fprintf(stdout, "%d" , 1);
    convert_to_binary_kmeans(*img);
    morphology(img);
    labeling(img);
    feature_extraction(img, mathcing_check);

}


void edge(struct Image *img){
    const int height = img->height;
    const int width = img->width;
    struct pixel *pixs = img->pixels;

    int16_t *input = (int16_t *)calloc(height * width, sizeof(int16_t));
    // int16_t *Gradiant =  (int16_t *)calloc(height * width, sizeof(int16_t));
    // int16_t *Gx = (int16_t *)calloc(height * width, sizeof(int16_t));
    // int16_t *Gy = (int16_t *)calloc(height * width, sizeof(int16_t));
    int16_t *output = (int16_t *)calloc(height * width, sizeof(int16_t));
    // int16_t *Non_max_suppression = calloc(height * width, sizeof(int16_t));

    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            input[h*width+w] = pixs[h*width+w].red;
        }
    }

    output = canny_edge_detection(input, height, width, 50, 45);
    // gussian_filter(input, output, height, width, 0.4);

    // const float Gx_derivative[] = { -1, 0, 1,
    //                                 -2, 0, 2,
    //                                 -1, 0, 1};
 
    // convolution(output, Gx, height, width, Gx_derivative, 3, 0);

    // const float Gy_derivative[] = { 1, 2, 1,
    //                                 0, 0, 0,
    //                                 -1,-2,-1};
 
    // convolution(output, Gy, height, width, Gy_derivative, 3,0);
    
    // for (int h = 1; h < height - 1; h++){
    //     for (int w = 1; w < width - 1; w++) {
    //         const int index = h * width + w;
    //         int16_t value = (int16_t)hypot(Gx[index], Gy[index]);
    //         Gradiant[index] =value;
    //     }
    // }
    // min_max_normalization(Gradiant, height, width);
    // float PI =  3.14159265358979323846264338327;
    // for (int h = 1; h < height - 1; h++){
    //     for (int w = 1; w < width - 1; w++) {
    //         const int c = h * width + w;    //center
    //         const int nn = c - width;       //north 
    //         const int ss = c + width;       //south
    //         const int ww = c + 1;           //west
    //         const int ee = c - 1;           //east
    //         const int nw = nn + 1;          //north-west
    //         const int ne = nn - 1;          //north-east
    //         const int sw = ss + 1;          //south-west
    //         const int se = ss - 1;          //south-east
 

    //         const float direction = (float)(fmod(atan2(Gy[c], Gx[c]) + PI, PI) / PI) * 8;
    //         //    (                           0 deg                                ) || (                                   45 deg                        ) || (                                   90 deg                        ) || (                               135 deg                            )
    //         if ( ((direction <= 1 || direction > 7) && Gradiant[c] > Gradiant[ee] && Gradiant[c] > Gradiant[ww]) || ((direction > 1 && direction <= 3) && Gradiant[c] > Gradiant[nw] && Gradiant[c] > Gradiant[se]) || ((direction > 3 && direction <= 5) && Gradiant[c] > Gradiant[nn] && Gradiant[c] > Gradiant[ss]) || ((direction > 5 && direction <= 7) && Gradiant[c] > Gradiant[ne] && Gradiant[c] > Gradiant[sw]))
    //             Non_max_suppression[c] = Gradiant[c];
    //         else
    //             Non_max_suppression[c] = 0;
    //     }
    // }
    // printf("he");
      
    // memset(output, 0, height*width*sizeof(int16_t));
    // memset(Gx, 0, height*width*sizeof(int16_t));
    // int *edges = (int *)Gx;
    
    // for(int h=1; h < height-1; h++){
    //     for(int w=1; w < width-1; w++){
    //         int position = h*width+w;
    //         if(Non_max_suppression[position] >= 50 && output[position]==0){
    //             output[position] = 255;
    //             int Number_of_edges = 1;
    //             edges[0] = position;

    //             while(Number_of_edges > 0){
    //                 Number_of_edges--;
    //                 const int edge_position = edges[Number_of_edges];
    //                 int neighbours[8];
    //                 neighbours[0] = edge_position - width;  //nn
    //                 neighbours[1] = edge_position + width;  //ss
    //                 neighbours[2] = edge_position - 1;      //ee
    //                 neighbours[3] = edge_position + 1;      //ww
    //                 neighbours[4] = neighbours[0] - 1;      //ne
    //                 neighbours[5] = neighbours[0] + 1;      //nw
    //                 neighbours[6] = neighbours[1] - 1;      //se
    //                 neighbours[7] = neighbours[1] + 1;      //sw

    //                 for(int posi; posi < 8; posi++){
    //                     if(Non_max_suppression[neighbours[posi]] >= 45 && output[neighbours[posi]] ==0 ){
    //                         output[neighbours[posi]] = 255;
    //                         edges[Number_of_edges] = neighbours[posi];
    //                         Number_of_edges++;
    //                     }
    //                 }

    //             }
    //         }
    //     }
    // }
    
    for(int h=0; h < height; h++){
        for(int w=0; w < width; w++){
            uint8_t value = output[h*width+w];
            set_pixel_value(pixs+h*width+w, value, value, value);
        }
    }
    
    // free(Gx);
    // free(Gy);
    // free(Gradiant);
    // free(Non_max_suppression);
    free(output);
}