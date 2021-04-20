#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Includes/img_operations.h"
#include "Includes/utils.h"
 
int main(int arg_count, char *arg_list[]){
    char image_path[200];
    struct Image img;
    int is_valid;
    int choise=0; 

    if (arg_count > 1){
        strncpy(image_path, arg_list[1], strlen(arg_list[1]));
        is_valid = Read_bmp(image_path, &img);
    }
    else{
        printf("Enter the path of your bmp image: ");
        scanf("%s", image_path);
        is_valid = Read_bmp(image_path, &img);

    }


    if(is_valid != -1){
        printf("\nImage has been Loaded What do you want to do with it?\n");
        while(choise != 6){
            printf("\n##############################-=[ Choose A Number ]=-##############################\n");
            printf("1 - Save It.\n");
            printf("2 - Convert To Gray.\n");
            printf("3 - Convert binary.\n");
            printf("4 - labeling\n");
            printf("5 - Reset All Changes.\n");
            printf("6 - Quit.\n");
            scanf("%d", &choise);
        
            if(choise < 1 || choise > 6){
                printf("\n[!!!] The number %d is an invalid selection.\n\n", choise);
            }
            else if(choise == 1){
                printf("Where do you want to save the file?(give the absolute path): ");
                scanf("%s", image_path);
                Write_bmp(image_path, img);
                
            }
            else if( choise == 2){
                convert_to_gray(&img);
            }
            else if( choise == 3){
                convert_to_binary_kmeans(img);
            }
            else if(choise == 4){
            labeling(&img);
            }
            else if( choise == 5){
                printf("Trying to reset changes...\n");
                int is_reset = Reset_changes(image_path, &img);
                if(is_reset == -1){
                    error("Unable to Reset changes!!!\n");
                }
                else{
                    printf("Changes have been Reseted.\n");
                }
            }
        }
    }
    printf("\nHave Good Day");
    free(img.pixels);
    free(img.header);
    return 0;
}

