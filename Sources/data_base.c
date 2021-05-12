
#include "../Includes/data_base.h"

char database_name[]={"mydb.dat"};

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


void rname(){
    char name[20];

    printf("\nEnter the New File Name:");
    while( (getchar()) != '\n');
    scanf("%[^\n]",name);

    rename(database_name,name);
    strcpy(database_name,name);
}

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

char mygetch(){
    char val;
    char rel;

    scanf("%c",&val);
    scanf("%c",&rel);
    return (val);
}
