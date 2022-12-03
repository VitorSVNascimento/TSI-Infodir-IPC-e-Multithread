#pragma once
#include <dirent.h>
#include <sys/types.h>
#define TAMANHO_MAXIMO_DO_PATH 500
#define SUCESSO 1
#define FALHA 0
#define DIRETORIO_INEXISTENTE -1

typedef struct dirent Diretorio;

typedef struct{
    char nomeDir;
    unsigned long long tamanhoEmBytes;
    unsigned int numeroDeSubdiretorios;
    unsigned int numeroDeArquivos;
}Infodir;

/*Efetua a abertura de um diretorio,
*/
Infodir lerDiretorio(char *path,DIR *dir);
int abrirDiretorio(char *path,DIR **diretorio);
int inicializa(int argc, char* argv[]);
int processoPai(DIR *dir,char *path);
int processoFilho(char *path,int segmentoID);
void inicializaStructInfodir(Infodir *infodir);
void somaStructInfodir(Infodir *destino,Infodir recurso,char *path);
pid_t criarProcesso();
