#pragma once
#include <dirent.h>
#define TAMANHO_MAXIMO_DO_PATH 500
#define SUCESSO 1
#define FALHA 0
#define DIRETORIO_INEXISTENTE -1

typedef struct dirent Diretorio;

/*Efetua a abertura de um diretorio,
*/
int abrirDiretorio(char *path,DIR **diretorio);
int inicializa(int argc, char* argv[]);
