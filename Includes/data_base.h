
#ifndef DATA_BASE_H
#define DATA_BASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct obj{
    int id;
    char name[50];
    double mean;
};

int main_database_operations();
void append_auto(int id, char name[50], double mean);
void append();
void display();
void displayAll();
void modify();
void del();
void search();

void rname();
void rremove();
int find_biggest_id();
char mygetch();



#endif