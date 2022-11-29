#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include<unistd.h> 
#include "infodir.h"

unsigned long lerDiretorio(char *path,DIR *dir){
    unsigned long total=0,retorno;
    Diretorio *diretorio;
    struct stat st;
    if(abrirDiretorio(path,&dir)==FALHA){
        return DIRETORIO_INEXISTENTE;
    }else{
        chdir(path);
        while((diretorio = readdir(dir))){
            if(strcmp(diretorio->d_name,"..") && strcmp(diretorio->d_name,".")){

                if(diretorio->d_type==DT_DIR){
                    retorno = lerDiretorio(diretorio->d_name,dir);
                    if(retorno!=DIRETORIO_INEXISTENTE){
                        chdir("..");
                        total+=retorno;
                    }
                }else{
                        total+=st.st_size;
                        printf("\n%s\t%ld",diretorio->d_name,st.st_size);
                    
                }
            }
        }
        return total;
    }
}

int abrirDiretorio(char *path,DIR **diretorio){
    
    *diretorio = opendir(path);
    if(!*diretorio)
        return FALHA;
    return SUCESSO;

}

int inicializa(int argc, char* argv[]){
    char path[TAMANHO_MAXIMO_DO_PATH];
    DIR *dir;
    unsigned long retorno;
    const int PRIMEIRO_PARAMETRO = 1;
    if(argc <= PRIMEIRO_PARAMETRO){
        strcpy(path,"./");
    }else
        strcpy(path,argv[PRIMEIRO_PARAMETRO]);

    dir = opendir(path);
    struct stat st;
    stat("teste/teste.txt",&st);
    printf("Tamanho teste %lu",st.st_size);

    retorno = lerDiretorio(path,dir);
    printf("\nRetorno total = %lu\n",retorno);
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[]){
    return inicializa(argc,argv);
}
