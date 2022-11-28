#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "infodir.h"

int inicializa(int argc, char* argv[]){
    char path[TAMANHO_MAXIMO_DO_PATH];
    DIR *dd;
    Diretorio *d;
    struct stat st;
    const int PRIMEIRO_PARAMETRO = 1;
    if(argc <= PRIMEIRO_PARAMETRO){
        strcpy(path,"./");
    }
    
    dd = opendir(path);
    if(dd==NULL){
        printf("Não abriu");
        return EXIT_FAILURE;
    }
    else
        while((d = readdir(dd))){
            stat(d->d_name,&st);
            printf("\n%s\t%ld\n",d->d_name,st.st_size);

        }
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]){
    return inicializa(argc,argv);
}
