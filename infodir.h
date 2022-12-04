#pragma once
#include <dirent.h>
#include <sys/types.h>
#define TAMANHO_MAXIMO_DO_PATH 500
#define SUCESSO 1
#define FALHA 0
#define DIRETORIO_INEXISTENTE -1

typedef struct dirent Diretorio;

typedef struct{
    char nomeDir[TAMANHO_MAXIMO_DO_PATH];
    unsigned long long tamanhoEmBytes;
    unsigned long numeroDeSubdiretorios;
    unsigned long numeroDeArquivos;
}Infodir;

/*Le o conteudo de um diretorio.
Recebe como parametro uma string com o caminho para o arquivo (*path).
Retorna uma struct infodir contendo o seu tamanho em bytes, total de subdiretorios e total de arquivos
caso o diretorio não exista retorna uma struct com valores zerados;
*/
Infodir lerDiretorio(char *path);
/*Efetua a abertura de um diretorio
Recebe como parametro um ponteiro para um endereço de memoria contendo uma struct DIR (**diretorio)
e o caminho para o diretório (*path).
Em caso de sucesso retorna SUCESSO e atribui o endereço de memoria do diretório ao endereço de memória da variavel diretorio
em caso de falha retorna FALHA
*/
int abrirDiretorio(char *path,DIR **diretorio);
/*Inicializa o caminho do diretório com o valor passado no parametro formal da função main.
caso não tenha sido passado nenhum exibe a msg de erro e retorna EXIT_FAILURE.
caso falhe ao abrir o diretório retorna EXIT_FAILURE.
e em caso de sucesso obtem o tamanho do diretório através dos metodos IPC e multithread e retorna EXIT_SUCCESS.
*/
int infodir(int argc, char* argv[]);
//Inicializa os valores numéricos da struct infodir passada por parametro com 0
void inicializaStructInfodir(Infodir *infodir);
/*Gera um segmento de memória compartilhada do tipo Infodir* e atribui esse endereço de memória para a veriavel
passa por parametro
Retorna o Id do segmento
*/
int criaSegmentoMemoriaCompartilhada(Infodir **infodir);
/*Soma os valores numéricos das structs infodir passadas por parametro. 
atribui os valores da soma a variavel *destino */
void somaStructInfodir(Infodir *destino,Infodir recurso);

int processoPai(DIR *dir,char *path);
/*Printa na tela um relatório com os dados do diretório e o tempo levado para calcular estes dados
*/
void printaRelatorio(Infodir *infodirPtr,char *metodo);
int processoFilho(char *path,int segmentoID);
pid_t criarProcesso();
/*Soma o valor em bytes do arquivo passado passado por parametro a struct infodir passada por parametro*/
void somarArquivoAStruct(Infodir *infodir,Diretorio *diretorio);