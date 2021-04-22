#include <stdio.h>
#include <stdlib.h>

#include "../Includes/linked_list.h"
#include "../Includes/pixels.h"


int Number_of_Nodes = 0;

int is_empty_list(struct pixel_node *head){
    if (head == NULL){
        return 1;
    }
    return 0;
}


void push(struct pixel_node **head, struct pixel pix){
    struct pixel_node *new_node = (struct pixel_node *)malloc(sizeof(struct pixel_node));
    copy_pixel_value(&new_node->pix, &pix);
    new_node->next = *head;
    *head = new_node;
    Number_of_Nodes += 1;
}


void print_list(struct pixel_node *head){
    if(is_empty_list(head)){
        printf("List is empty!!!\n");
    }
    else{
        struct pixel_node *first = (head);
        while(first != NULL){
            printf("red: %d, green: %d, blue: %d\n", first->pix.red, first->pix.green, first->pix.blue);
            first = first->next;
        }
    }
}


void add_if_not_exists(struct pixel_node **head, struct pixel pix){
    struct pixel_node *first = *head;
    int exists = 0;
    while (first != NULL){
        exists = compare_two_pixels(first->pix , pix);
        if(exists){
            break;
        }
        first = first->next;
    }
    if(exists == 0 ){
        push(head, pix);
    }
}


void free_all(struct pixel_node **head){
    Number_of_Nodes = 0;
    if(is_empty_list(*head)){
        printf("All clear.\n");
    }
    else{
        struct pixel_node *tmp = (*head);
        while ( tmp != NULL ){
            (*head) = (*head)->next;
            free(tmp);
            tmp = (*head);
        }
        
    }
}


struct pixel *get_node_pixel(struct pixel_node *head, int i){
    if(i < Number_of_Nodes){
        struct pixel_node *tmp = head;
        while(i != 0){
            tmp = tmp->next;
            i--;
        }
        if(tmp!=NULL){
            return &(tmp->pix);
        }
    }
    else{
        return NULL;
    }
}