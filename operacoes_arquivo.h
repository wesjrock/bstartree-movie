#ifndef OPERACOES_ARQUIVO_H
#define OPERACOES_ARQUIVO_H

#include <stdio.h>
#include <stdlib.h>

/** struct Registro:
    Estrutura para armazenar um registro em memória
    Neste programa, um registro é carregado em memória
    utilizando essa estrutura, printado, e a memória é liberada
**/
typedef struct reg {
    int id;
    int ano;
    int duracao;
    char* titulo;
    char* descr;
    char* pais;
    char* genero;
} Registro;

/** Lê todas as letras encontradas,a partir da posição atual do arquivo até um marcador qualquer, e retorna-as como uma string.*/
char* arqTok(FILE* arquivo, char marcador);

/** Retorna a quantidade total de registros no arquivo **/
int contaMarcador(FILE* arquivo, char marcador);

long getTamArquivo(FILE* fp);

/** Retorna os proximos 4 bytes lidos do arquivo, como int **/
int nextInt(FILE* arquivo);

int arqExiste(char* nomeArquivo);




#endif // OPERACOES_ARQUIVO_H
