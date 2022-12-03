#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h> 
#include <locale.h> 
#include "infodir.h"

Infodir lerDiretorio(char *path,DIR *dir){
    Diretorio *diretorio;
    struct stat st;
    Infodir infodir,retorno;
    inicializaStructInfodir(&infodir);
    if(abrirDiretorio(path,&dir)==FALHA){
        infodir.numeroDeArquivos=DIRETORIO_INEXISTENTE;
        return infodir;
    }else{
        chdir(path);
        while((diretorio = readdir(dir))){
            if(strcmp(diretorio->d_name,"..") && strcmp(diretorio->d_name,".") ){

                if(diretorio->d_type==DT_DIR){
                    infodir.numeroDeSubdiretorios++;
                    retorno = lerDiretorio(diretorio->d_name,dir);
                    if(retorno.numeroDeArquivos!=DIRETORIO_INEXISTENTE){
                        chdir("..");
                        somaStructInfodir(&infodir,retorno,path);
                        
                    }
                }else{
                        stat(diretorio->d_name,&st);
                        infodir.tamanhoEmBytes+=st.st_size;
                        infodir.numeroDeArquivos++;
                    
                }
            }
        }
        return infodir;
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
    
    
    const int PRIMEIRO_PARAMETRO = 1;
    if(argc <= PRIMEIRO_PARAMETRO){
        strcpy(path,"./");
    }else
        strcpy(path,argv[PRIMEIRO_PARAMETRO]);

    if(abrirDiretorio(path,&dir)==SUCESSO)
        processoPai(dir,path);
    
    
    return EXIT_SUCCESS;
}

void inicializaStructInfodir(Infodir *infodir){
    infodir->numeroDeArquivos = 0;
    infodir->numeroDeSubdiretorios = 0;
    infodir->tamanhoEmBytes = 0;
}

int processoPai(DIR *dir,char *path){
    int totalFilhos=0;
    Diretorio *diretorio;
    struct stat st;
    int segmentoID = shmget(IPC_PRIVATE, sizeof(Infodir), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    Infodir *infodirPtr = malloc(sizeof(Infodir));
    infodirPtr= (Infodir *)shmat(segmentoID, NULL, 0);
    inicializaStructInfodir(infodirPtr);
   
    chdir(path);
    while((diretorio = readdir(dir))){
        if(strcmp(diretorio->d_name,"..") && strcmp(diretorio->d_name,".") ){

            if(diretorio->d_type==DT_DIR){
                infodirPtr->numeroDeSubdiretorios++;
                pid_t pid = criarProcesso();
                if(!pid)
                    processoFilho(diretorio->d_name,segmentoID);
                else
                    totalFilhos++;
            }else{
                    stat(diretorio->d_name,&st);
                    infodirPtr->tamanhoEmBytes+=st.st_size;
                    infodirPtr->numeroDeArquivos++;
                
            }
        }
    }

    while(totalFilhos){
        wait(NULL);
        totalFilhos--;
    }
    
    
    printf("\nRetorno total = %llu\n",infodirPtr->tamanhoEmBytes);
    printf("\nSubdiretório total = %u\n",infodirPtr->numeroDeSubdiretorios);
    printf("\nArquivos total = %u\n",infodirPtr->numeroDeArquivos);
    shmctl(segmentoID, IPC_RMID, NULL); 
    //free(infodirPtr);
    return EXIT_SUCCESS;
}

int processoFilho(char *path,int segmentoID){
    printf("\nEntrei no filho = %s\n",path);

    DIR *dir;
    Infodir retorno;
    abrirDiretorio(path,&dir);
    //printf("\nAntes %s\n",path);
    retorno = lerDiretorio(path,dir);
    //printf("\nDepois %s\n",path);
    Infodir* infodirPtr= (Infodir *)shmat(segmentoID, NULL, 0);
    if(retorno.numeroDeArquivos!=DIRETORIO_INEXISTENTE){
         chdir("..");
        somaStructInfodir(infodirPtr,retorno,path);      
    }
   // printf("\nValor do criação %llu\n",infodirPtr->tamanhoEmBytes);
    //printf("\nValor do infodir %llu\n",infodirPtr->tamanhoEmBytes);
    shmdt(infodirPtr);

    exit(EXIT_SUCCESS);
    // return 0;
}

pid_t criarProcesso() {
    // Cria o processo filho.
    pid_t pid = fork();

    // Verifica se ocorreu um erro na criação do processo filho. 
    if (pid < 0)
    {
        printf("\nERRO: o processo filho não foi criado.\n\n");
        exit(EXIT_FAILURE);
    }
    return pid;
}

void somaStructInfodir(Infodir *destino,Infodir recurso,char *path){
    destino->numeroDeArquivos+=recurso.numeroDeArquivos;
    destino->numeroDeSubdiretorios+=recurso.numeroDeSubdiretorios;
    destino->tamanhoEmBytes+=recurso.tamanhoEmBytes;
}

int main(int argc, char* argv[]){
    return inicializa(argc,argv);
}