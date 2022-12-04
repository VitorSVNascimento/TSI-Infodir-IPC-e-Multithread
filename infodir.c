#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h> 
#include <locale.h> 
#include "infodir.h"

Infodir lerDiretorio(char *path){
    DIR *dir;
    Diretorio *diretorio;
    Infodir infodir,retorno;
    inicializaStructInfodir(&infodir);
    if(abrirDiretorio(path,&dir)==FALHA)
        return infodir;
    
    chdir(path);
    while((diretorio = readdir(dir))){
        if(!strcmp(diretorio->d_name,"..") || !strcmp(diretorio->d_name,".") )
            continue;

        if(diretorio->d_type!=DT_DIR){
            somarArquivoAStruct(&infodir,diretorio);
            continue;
        }

        infodir.numeroDeSubdiretorios++;
        retorno = lerDiretorio(diretorio->d_name);
        chdir("..");
        somaStructInfodir(&infodir,retorno);
         
        
    }
    closedir(dir);
    return infodir;
    
}

int abrirDiretorio(char *path,DIR **diretorio){
    
    *diretorio = opendir(path);
    if(!*diretorio)
        return FALHA;
    return SUCESSO;

}

int infodir(int argc, char* argv[]){
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

int criaSegmentoMemoriaCompartilhada(Infodir **infodir){
    int segmentoID = shmget(IPC_PRIVATE, sizeof(Infodir), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    *infodir =  (Infodir *)shmat(segmentoID, NULL, 0);
    return segmentoID;
}

void somarArquivoAStruct(Infodir *infodir,Diretorio *diretorio){
    struct stat st;
    stat(diretorio->d_name,&st);
    infodir->tamanhoEmBytes+=st.st_size;
    infodir->numeroDeArquivos++;
    return;
}

int processoPai(DIR *dir,char *path){
    char metodo[] = {"IPC - Interprocess Communication"};
    
    int totalFilhos=0;
    Diretorio *diretorio;
    Infodir *memoriaCompartilhada = NULL;
    int segmentoID = criaSegmentoMemoriaCompartilhada(&memoriaCompartilhada);
    inicializaStructInfodir(memoriaCompartilhada);
   
    chdir(path);
    while((diretorio = readdir(dir))){
        if(!strcmp(diretorio->d_name,"..") || !strcmp(diretorio->d_name,".") )
            continue;

        /*Caso o arquivo não seja um diretorio aciona a função de soma 
        e pula para a proxima iteração do loop*/
        if(diretorio->d_type!=DT_DIR){
            somarArquivoAStruct(memoriaCompartilhada,diretorio); 
            continue;
        }

        memoriaCompartilhada->numeroDeSubdiretorios++;
        pid_t pid = criarProcesso();
        if(!pid)
            processoFilho(diretorio->d_name,segmentoID);
        else
            totalFilhos++;
    }

    //Espera até que todos os filhos terminem a execução para obter o relatório
    while(totalFilhos){
        wait(NULL);
        totalFilhos--;
    }
    getcwd(memoriaCompartilhada->nomeDir,TAMANHO_MAXIMO_DO_PATH);
    printaRelatorio(memoriaCompartilhada,metodo);
    shmctl(segmentoID, IPC_RMID, NULL); 
    return EXIT_SUCCESS;
}

void printaRelatorio(Infodir *infodir,char *metodo){

    printf("\n-Método: %s",metodo);
    printf("\nDiretório: %s\n",infodir->nomeDir);
    printf("\nConteúdo do diretório");
    printf("\n\tArquivos = %lu",infodir->numeroDeArquivos);
    printf("\n\tSubdiretórios = %lu",infodir->numeroDeSubdiretorios);
    printf("\n\tTamanho do diretório = %llu\n",infodir->tamanhoEmBytes);
}

int processoFilho(char *path,int segmentoID){
    Infodir retorno;
    
    retorno = lerDiretorio(path);
    Infodir* memoriaCompartilhada= (Infodir *)shmat(segmentoID, NULL, 0);
    somaStructInfodir(memoriaCompartilhada,retorno);      
    shmdt(memoriaCompartilhada);
    
    exit(EXIT_SUCCESS);
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

void somaStructInfodir(Infodir *destino,Infodir recurso){
    destino->numeroDeArquivos+=recurso.numeroDeArquivos;
    destino->numeroDeSubdiretorios+=recurso.numeroDeSubdiretorios;
    destino->tamanhoEmBytes+=recurso.tamanhoEmBytes;
}

int main(int argc, char* argv[]){
    return infodir(argc,argv);
}