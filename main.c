#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>

char pathOrigen[100];
char pathDestino[100];


void createFolder (){

}
bool readFolder(char *path){
    FILE *fp = fopen(path, "r");
    bool is_exist = false;
    if (fp != NULL){
        is_exist = true;
        fclose(fp); // close the file
    }
    return is_exist;
}

int main (void){
    printf("Por favor indique la ubicación de la carpeta que se desea copiar: ");
    scanf ("%s", pathOrigen);
    if(readFolder(pathOrigen)){
        printf("Por favor indique la ubicación de la carpeta donde desea pegar los archivos: ");
        scanf ("%s", pathDestino);
        //int result = mkdir("/home/bryan/Desktop/Test", 0777);
    }else{
        printf("La ubicación ingresada no existe");
    }
    return 0;
}