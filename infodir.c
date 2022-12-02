#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <unistd.h> 
#include <locale.h> 
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
            if(strcmp(diretorio->d_name,"..") && strcmp(diretorio->d_name,".") ){

                if(diretorio->d_type==DT_DIR){
                    retorno = lerDiretorio(diretorio->d_name,dir);
                    if(retorno!=DIRETORIO_INEXISTENTE){
                        chdir("..");
                        total+=retorno;
                        
                    }
                }else{
                        stat(diretorio->d_name,&st);
                        total+=st.st_size;
                        printf("\n%s\t%ld\n%d",diretorio->d_name,st.st_size,diretorio->d_type);
                    
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
    
    
    const int PRIMEIRO_PARAMETRO = 1;
    if(argc <= PRIMEIRO_PARAMETRO){
        strcpy(path,"./");
    }else
        strcpy(path,argv[PRIMEIRO_PARAMETRO]);

    if(abrirDiretorio(path,&dir)==SUCESSO)
        processoPai(dir,path);
    
    
    return EXIT_SUCCESS;
}

int processoPai(DIR *dir,char *path){
    int teste=0;
    Diretorio *diretorio;
    struct stat st;
    int segmentoID = shmget(IPC_PRIVATE, sizeof(Infodir), IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
    Infodir *infodirPtr = malloc(sizeof(Infodir));
    infodirPtr= (Infodir *)shmat(segmentoID, NULL, 0);
    Infodir infodir;
   
    chdir(path);
    while((diretorio = readdir(dir))){
        if(strcmp(diretorio->d_name,"..") && strcmp(diretorio->d_name,".") ){

            if(diretorio->d_type==DT_DIR){
                teste=1;
                pid_t pid = criarProcesso();
                if(!pid)
                    processoFilho(diretorio->d_name,segmentoID);
            }else{
                    stat(diretorio->d_name,&st);
                    infodir.tamanhoEmBytes+=st.st_size;
                    printf("\n%s\t%ld\n%d\n",diretorio->d_name,st.st_size,diretorio->d_type);
                
            }
        }
    }
    if(teste)
        infodir.tamanhoEmBytes += infodirPtr->tamanhoEmBytes;
    printf("\nRetorno total = %llu\n",infodir.tamanhoEmBytes);
    shmctl(segmentoID, IPC_RMID, NULL); 
    return EXIT_SUCCESS;
}

int processoFilho(char *path,int segmentoID){
    printf("\nEntrei no filho = %s\n",path);

    DIR *dir;
    unsigned long retorno,total=0;
    abrirDiretorio(path,&dir);
    printf("\nAntes %s\n",path);
    retorno = lerDiretorio(path,dir);
    printf("\nDepois %s\n",path);
    if(retorno!=DIRETORIO_INEXISTENTE){
         chdir("..");
        total+=retorno;       
    }
    printf("\nRetorno %lu\n",total);
    Infodir* infodirPtr= (Infodir *)shmat(segmentoID, NULL, 0);
    printf("\nValor do criação %llu\n",infodirPtr->tamanhoEmBytes);
    infodirPtr->tamanhoEmBytes+=total;
    printf("\nValor do infodir %llu\n",infodirPtr->tamanhoEmBytes);
    shmdt(infodirPtr);

    exit(EXIT_SUCCESS);
    // return 0;
}

pid_t criarProcesso() {
    // Cria o processo filho.
    pid_t pid = fork();

    if(pid>0)
        wait(NULL);
    // Verifica se ocorreu um erro na criação do processo filho. 
    if (pid < 0)
    {
        printf("\nERRO: o processo filho não foi criado.\n\n");
        exit(EXIT_FAILURE);
    }
    return pid;
}


int main(int argc, char* argv[]){
    return inicializa(argc,argv);
}