#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "../Includes/linked_list.h"

int main ()
{
  struct pixel_node *head = NULL;
  // head->next = NULL;

  struct pixel pix = {251, 55 , 152};
  struct pixel pix2 = {1, 2 , 3};
  struct pixel pix3 = {4, 5 , 6};
  struct pixel pix4 = {7, 8 , 9};
  struct pixel pix5 = {7, 38 , 9};
  struct pixel pix6 = {1, 2 , 3};
  struct pixel pix7 = {65, 75 , 85};
  struct pixel pix8 = {89, 8 , 9};
  // push(&head, pix);
  // push(&head, pix2);
  // push(&head, pix3);
  // push(&head, pix4);
  extern int Number_of_Nodes;
  add_if_not_exists(&head, pix5);
  add_if_not_exists(&head, pix7);
  add_if_not_exists(&head, pix8);
  add_if_not_exists(&head, pix6);
  print_list(head);
  printf("%d\n", Number_of_Nodes);
  struct pixel a;
  struct pixel *b = get_node_pixel(head, 3);
  copy_pixel_value(&a, b);
  printf("%d %d %d\n", a.red, a.green, a.blue );
  free_all(&head);
  // print_list(head);
  // printf("%d\n", Number_of_Nodes);
  
}
