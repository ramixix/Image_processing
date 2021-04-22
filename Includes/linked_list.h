#ifndef LINKED_PIXLES
#define LINKED_PIXLES

#include "pixels.h"

struct pixel_node{
    struct pixel pix;
    struct pixel_node *next;
};

int is_empty_list(struct pixel_node *head);
void push(struct pixel_node **head, struct pixel pix);
void print_list(struct pixel_node *head);
void add_if_not_exists(struct pixel_node **head, struct pixel pix);
void free_all(struct pixel_node **head);
struct pixel *get_node_pixel(struct pixel_node *head, int i);
#endif