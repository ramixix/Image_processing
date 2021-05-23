#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "Includes/img_operations.h"
#include "Includes/utils.h"
#include "Includes/data_base.h"
 
 #define PATH_LENGTH 200

int main(int arg_count, char *arg_list[]){
    char image_path[PATH_LENGTH];
    char save_path[PATH_LENGTH];
    struct Image img;
    int is_valid;
    int choise=0;
    srand(time(NULL));

    // get image path to open in two ways : 1. as command line argument while compling the code 2. promp user to etner it after running code.
    if (arg_count > 1){
        strncpy(image_path, arg_list[1], PATH_LENGTH-1);
        is_valid = Read_bmp(image_path, &img);
    }
    else{
        printf("Enter the path of your bmp image: ");
        scanf("%199s", image_path);
        is_valid = Read_bmp(image_path, &img);

    }

    // start asking user what they want to do with opened image
    if(is_valid != -1){
        printf("\nImage has been Loaded What do you want to do with it?\n");
        while(choise != 11){
            printf("\n##############################-=[ Choose A Number ]=-##############################\n");
            printf("1 - Convert To Gray.\n");
            printf("2 - Convert Binary.\n");
            printf("3 - morphology\n");
            printf("4 - Labeling\n");
            printf("5 - Bounding Box\n");
            printf("6 - Extract Featrues.\n");
            printf("7 - Object detection.\n");
            printf("8 - Save It.\n");
            printf("9 - Reset All Changes.\n");
            printf("10 - Connect To DataBase.\n");
            printf("11 - Quit.\n");
            printf("====================================================================================\n\n");
            printf("Please Enter Your Choise: ");
            scanf("%d", &choise);
            while ((getchar()) != '\n');

            if(choise < 1 || choise > 11){
                printf("\n[!!!] The number %d is an invalid selection.\n\n", choise);
            }
            else if( choise == 1){
                convert_to_gray(&img);
            }
            else if( choise == 2){
                convert_to_binary_kmeans(img);
            }

            else if(choise == 3){
                morphology(&img);
            }

            else if(choise == 4){
                labeling(&img);
            }
            else if(choise == 5){
                bounding_box(&img);
            }
            else if(choise == 6){
                feature_extraction(&img);
            }

            else if(choise == 8){
                printf("Where do you want to save the file?(give the absolute path): ");
                scanf("%s", save_path);
                Write_bmp(save_path, img);
                
            }

            else if( choise == 9){
                printf("Trying to reset changes...\n");
                int is_reset = Reset_changes(image_path, &img);
                if(is_reset == -1){
                    error("Unable to reset changes!!!\n");
                }
                else{
                    printf("Changes have been Reseted.\n");
                }
            }

            else if(choise == 10){
                main_database_operations();
            }
        }
    }
    printf("\nHave Good Day");
    // free the amount of memory that was allocated for image header and image data here after all operations are done
    free(img.pixels);
    free(img.header);
    return 0;
}

