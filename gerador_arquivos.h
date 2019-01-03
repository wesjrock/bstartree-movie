#ifndef GERADOR_ARQUIVOS_H
#define GERADOR_ARQUIVOS_H
#include <stdlib.h>
#include <stdio.h>
#include "operacoes_arquivo.h"
#include "utilitarios.h"
/** Guarda o numero de registros no arquivo de dados */
extern int N_REGS;

/** Copia o ultimo n�mero lido no arquivo de orgigem (string) no arquivo destino, convertido pra int */
void copiarUltimoIntLido(FILE* arq_origem,FILE* arq_destino, int nDigitos);

/** Copia os "n" bytes anteriores � posi��o atual do ponteiro de arquivo de origem, no arquivo de destino */
void copiarUltimaStringLida(FILE* arq_origem,FILE* arq_destino, int n);

FILE* createIDXFile(FILE* arq_dados);

/** Avan�a o ponteiro do arquivo at� a posi��o que come�a o primeiro campo do filme n no arquivo de filmes */
void fSeekFilme(int n, FILE* arq_filme);

/** Escolhe 100 filmes aleat�rios dentro todos os filmes presentes no arquivo de filmes,
    atribui IDs aleat�rios a esses filmes e escreve no arquivo de dados.  */
FILE* gerarArqDados();

#endif // GERADOR_DADOS_H
