
#include "../Includes/data_base.h"
#include "../Includes/utils.h"

// name of database file
char database_name[]={"mydb.dat"};


// main function that is used to interact with user when they want to connect to database
int main_database_operations(){
    int choise;

    while(1){
        system("clear");

        printf("==================object Management System=============\n\n");

        printf("1. Append\n\n");
        printf("2. Modify\n\n");
        printf("3. Delete\n\n");
        printf("4. Search\n\n");
        printf("5. Display\n\n");
        printf("6. Display All\n\n");
        printf("7. Rename\n\n");
        printf("8. Delete File\n\n");
        printf("0. Exit\n\n");

        printf("========================================================\n\n");

        printf("\nPlease enter your Choice:");
        scanf("%d",&choise);

        switch(choise){
            case 1: append();
            break;

            case 2: modify();
            break;

            case 3: del();
            break;

            case 4: search();
            break;

            case 5: display();
            break;

            case 6: displayAll();
            break;

            case 7: rname();
            break;

            case 8: rremove();
            break;

            case 0: exit(0);
        }

        mygetch();
    }
    return 0;
}


// append new entity to database
void append(){
    FILE *fp;
    struct obj object;

    fp=fopen(database_name,"ab");

    printf("\nEnter ID:");
    scanf("%d",&object.id);
    printf("\nEnter name:");
    scanf("%s",object.name);
    printf("\nEnter mean:");
    scanf("%lf",&object.mean);
    fwrite(&object,sizeof(object),1,fp);

    fclose(fp);
}


// rename the database file
void rname(){
    char name[20];

    printf("\nEnter the New File Name:");
    while( (getchar()) != '\n');
    scanf("%[^\n]",name);

    rename(database_name,name);
    strcpy(database_name,name);
}


// remove database file. when user call this function we first ask them if they want to take a backup.
void rremove(){
    FILE *fp,*fp1;
    struct obj t;

    char name[20];
    char val[20];

    printf("\nDo you want to make copy of it (Y/N):");
    scanf("%s",val);

    if(strcmp(val,"Y")==0){
        printf("\nEnter the New File Name:");
        while( (getchar()) != '\n');
        scanf("%[^\n]",name);

        fp=fopen(name,"wb");
        fp1=fopen(database_name,"rb");

        while(1){
            fread(&t,sizeof(t),1,fp1);

            if(feof(fp1)){
                break;
            }
            fwrite(&t,sizeof(t),1,fp);
        }

        fclose(fp);
        fclose(fp1);

        remove(database_name);

        strcpy(database_name,name);
    }
    else{
        remove(database_name);
    }
}


// modify an entity inside database. find the entity using its id number.
void modify(){
    FILE *fp,*fp1;
    struct obj t,object;
    int id,found=0,count=0;

    fp=fopen(database_name,"rb");
    fp1=fopen("temp.dat","wb");

    printf("\nEnter the obj ID you want to Modify:");
    scanf("%d",&id);

    while(1){
        fread(&t,sizeof(t),1,fp);

        if(feof(fp)){
            break;
        }
        if(t.id==id){
            found=1;
            printf("\nEnter object ID:");
            scanf("%d",&t.id);
            while( (getchar()) != '\n');

            printf("\nEnter object Name:");
            scanf("%s",t.name);
            printf("\nEnter object mean:");
            scanf("%lf",&t.mean);
            fwrite(&t,sizeof(t),1,fp1);
        }
        else{
            fwrite(&t,sizeof(t),1,fp1);
        }
    }
    fclose(fp);
    fclose(fp1);

    if(found==0){
        printf("Sorry No Record Found\n\n");
    }
    else{
        fp=fopen(database_name,"wb");
        fp1=fopen("temp.dat","rb");

        while(1){
            fread(&t,sizeof(t),1,fp1);

            if(feof(fp1)){
                break;
            }
            fwrite(&t,sizeof(t),1,fp);
        }

    }
    fclose(fp);
    fclose(fp1);
}


// delete an entity using its id.
void del()
{
    FILE *fp,*fp1;
    struct obj t,object;
    int id,found=0,count=0;

    fp=fopen(database_name,"rb");
    fp1=fopen("temp.dat","wb");

    printf("\nEnter the obj ID you want to Delete:");
    scanf("%d",&id);

    while(1){
        fread(&t,sizeof(t),1,fp);

        if(feof(fp)){
            break;
        }
        if(t.id==id){
            found=1;
        }
        else{
            fwrite(&t,sizeof(t),1,fp1);
        }
    }
    fclose(fp);
    fclose(fp1);

    if(found==0){
        printf("Sorry No Record Found\n\n");
    }
    else{
        fp=fopen(database_name,"wb");
        fp1=fopen("temp.dat","rb");
        while(1){
            fread(&t,sizeof(t),1,fp1);

            if(feof(fp1)){
                break;
            }
            fwrite(&t,sizeof(t),1,fp);
        }
    }
    fclose(fp);
    fclose(fp1);
}


// display an entity to screen.
void display(){
    FILE *fp;
    struct obj t;
    int id,found=0;

    fp=fopen(database_name,"rb");

    printf("\nEnter the obj ID:");
    scanf("%d",&id);

    while(1){
        fread(&t,sizeof(t),1,fp);

        if(feof(fp)){
            break;
        }
        if(t.id==id){
            found=1;
            printf("\n========================================================\n\n");
            printf("\t\t object Details of %d\n\n",t.id);
            printf("========================================================\n\n");

            printf("Name\tmean\n\n");

            printf("%s\t",t.name);
            printf("%lf\t\n\n",t.mean);

            printf("========================================================\n\n");
        }
    }
    if(found==0){
        printf("\nSorry No Record Found");
    }
    fclose(fp);
}


// it display an entity, by searching it name inside databas.
void search(){
    FILE *fp;
    struct obj t;
    int found=0;
    char name[20];

    fp=fopen(database_name,"rb");

    printf("\nEnter the object Name:");
    scanf("%s",&name);

    while(1){
        fread(&t,sizeof(t),1,fp);

        if(feof(fp)){
            break;
        }
        if(strcmp(name,t.name)==0){
            printf("\n========================================================\n\n");
            printf("\t\t object Details of %d\n\n",t.id);
            printf("========================================================\n\n");

            printf("Name\tmean\n\n");

            printf("%s\t",t.name);
            printf("%lf\t\n\n",t.mean);

            printf("========================================================\n\n");
        }
    }
    if(found==0){
        printf("\nSorry No Record Found");
    }
    fclose(fp);
}


// display all entities inside database
void displayAll(){
    FILE *fp;
    struct obj t;

    fp=fopen(database_name,"rb");

    printf("\n========================================================\n\n");
    printf("\t\t All object Details\n\n");
    printf("========================================================\n\n");

    printf("ID\tName\tmean\n\n");

    while(1){
        fread(&t,sizeof(t),1,fp);

        if(feof(fp)){
            break;
        }
        printf("%d\t",t.id);
        printf("%s\t",t.name);
        printf("%lf\t\n\n",t.mean);

    }
    printf("========================================================\n\n");

    fclose(fp);
}


// find the biggest id by going through each entity and return biggest id of entitys. otherwise returns -1
int find_biggest_id(){
    FILE *fp = fopen(database_name, "rb");
    int biggest_id = -1;
    struct obj temp;

    while(1){
        fread(&temp, sizeof(struct obj), 1, fp);
        if(feof(fp)){
            break;
        }
        biggest_id = temp.id;
    }
    fclose(fp);
    return (biggest_id + 1);
}


// append an entity. this is used inside program where there is no direct interact between user and database
void append_auto(int id, char name[50], double mean){
    struct obj object;
    object.id = id;
    strncpy(object.name, name, 50);
    object.mean = mean;

    FILE *fp;
    fp = fopen(database_name,"ab");
    fwrite(&object,sizeof(object),1,fp);
    fclose(fp);
}


char *minimum_distance_obj_detection(double mean, char name[256]){
    FILE *fp = fopen(database_name, "rb");
    struct obj entity;
    double minimum_distance = __DBL_MAX__;
    while(1){
        fread(&entity, sizeof(entity), 1, fp);
        if( feof(fp)){
            break;
        }

        double dist = distant(mean, entity.mean);
        if(dist < minimum_distance){
            minimum_distance = dist;
            strncpy(name, entity.name, 255);
        }

    }
    // if database is empty
    if(minimum_distance ==__DBL_MAX__){
        fclose(fp);
        strncpy(name, "No match (either your database is empty or you ruin the code)", 62);
        return name;
    }
    else{
        fclose(fp);
        return name;
    }
}


char mygetch(){
    char val;
    char rel;

    scanf("%c",&val);
    scanf("%c",&rel);
    return (val);
}
