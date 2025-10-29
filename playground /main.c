#include <stdio.h>

int main(){

    FILE * filepointer;

    filepointer = fopen("text.txt", "w");

    if (filepointer == NULL){
        printf("Nie mozna otworzyc pliku");
        return 1; 
    }
    fclose(filepointer);
    return 0;
}