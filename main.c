/*
Trabalho de Organizacao de Arquivos - Parte 2
Prof(a): Cristina Dutra de Aguiar Ciferri
Estagiário PAE: Rayner de Melo Pires

ICMC USP São Carlos

Alunos (Turma A):

Gabriel Pinheiro de Carvalho (7960727)
Jorge Luiz da Silva Vilaça (9066491)
Raul Procopio (9277200)
Wesley Tiozzo (8077925)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "operacoes_arquivo.h"
#include "gerador_arquivos.h"
#include "arvore_b_estrela.h"

#define SAIR 0


/************************************* Protótipos de Funções *******************************************/

/** Insere o registro passado como argumento no arquivo de dados e cria uma entrada no arquivo de indice para o mesmo */
void cadastrarReg(Registro* reg, FILE* arq_dados, FILE* idx_file);

/** Aloca memória para os campos do tipo string, e retorna a string alocada com o texto passado em buffer **/
char* criarCampoFrom(char buffer[]);

/** Libera a memória utilizada por um registro **/
void freeReg(Registro* reg);

/** Retorna o registro do arquivo com o ID que foi passado por parametro, utilizando a busca na arvore*/
Registro* getReg(int id, FILE* arq_dados, FILE* idx_file);

/** Inicializa o programa, criando o arquivo de dados e índice se não existirem e popula a arvore com os indices, a partir do arquivo de indice */
void initProgram(FILE** arq_dados, FILE** idx_file);

/** Lista todos os registros */
void listAllRegs(FILE* arq_dados, FILE* idx_file);

/** Printa um registro com todos os seus campos */
void printReg(int regID, FILE* arq_dados, FILE* idx_file);


/************************************* MAIN *******************************************/
int main()
{
    FILE* arq_dados = NULL;
    FILE* idx_file = NULL;

    initProgram(&arq_dados, &idx_file);

    int command; //Variavel associada a escolha da funcao

    while (command != SAIR){
        printf("-------------------------------------------------");
        printf("\nO que deseja fazer agora?\n");
        printf("1) Cadastrar novo filme\n");
        printf("2) Buscar filme\n");
        printf("3) Listar todos os filmes\n");
        printf("4) Mostrar Arvore de Indices\n");
        printf("0) Sair\n\n");
        printf("Enter your choice : ");
        scanf("%d", &command);
        printf("-------------------------------------------------\n");
         fflush(stdin);

        switch (command)
        {
            case 1:
            {
                Registro reg;
                char buffer[1024];

                printf("\nTitulo: ");
                fgets (buffer, 1024, stdin);
                fflush(stdin);
                reg.titulo = criarCampoFrom(buffer);

                printf("Descricao: ");
                fgets (buffer, 1024, stdin);
                fflush(stdin);
                reg.descr = criarCampoFrom(buffer);

                printf("Producao: ");
                fgets (buffer, 1024, stdin);
                fflush(stdin);
                reg.pais = criarCampoFrom(buffer);

                printf("Ano: ");
                scanf("%d", &reg.ano);
                fflush(stdin);

                printf("Duracao: ");
                scanf("%d", &reg.duracao);
                fflush(stdin);

                printf("Genero: ");
                fgets (buffer, 1024, stdin);
                fflush(stdin);
                reg.genero = criarCampoFrom(buffer);

                //Insere o registro passado como argumento no arquivo de dados e cria uma entrada no arquivo de indice para o mesmo
                cadastrarReg(&reg,arq_dados,idx_file);
            } break;
            case 2:
            {
                int index; //Variavel associada ao indice do filme buscado
                printf("Digite o ID do filme a ser buscado (0 - %d):\nR:", N_REGS - 1);
                scanf("%d", &index);
                printReg(index, arq_dados, idx_file);
                //Buscar filme
            }break;
            case 3:
            {
                //Listar filmes
                listAllRegs(arq_dados,idx_file);
                break;
            }
            case 4:
            {
                display(root,1);
            }break;
            default:
            //Sair
            break;
        }
    }

    if (arq_dados)
    {
        fclose(arq_dados);
    }
    if (idx_file)
    {
         fclose(idx_file);
    }

    return 0;
}



/************************************* Implementação das Funções *******************************************/


/** Inicializa o programa, criando o arquivo de dados e índice se não existirem e popula a arvore com os indices, a partir do arquivo de indice */
void initProgram(FILE** arq_dados, FILE** idx_file)
{
   int existeArqDados = arqExiste("DADOS.bin");
    int existeArqIDX = arqExiste("INDICES.idx");

    if(existeArqDados)
    {
        *arq_dados = fopen("DADOS.bin", "rb+");
    }
    else
    {
         //Gera o arquivo de dados, caso ele não exista ou senão simplesmente retorna o ponteiro para o arquivo de dados já existente como leitura e escrita.
        *arq_dados = gerarArqDados();
    }

   if(*arq_dados)
    {
        N_REGS = contaMarcador(*arq_dados,'#') -1; // Atualiza o numero total de registros
         printf("Total de Registros: %d\n", N_REGS);
        printf("Arquivo de dados... OK\n");
    }
    else
    {
        printf("Falha ao abrir arquivo de dados existente\n");
        exit(1);
    }


    // Se  o arquivo de dados e de indices existirem
    if (existeArqDados && existeArqIDX)
    {
        // Tenta abrir o arquivo de indices
        *idx_file = fopen("INDICES.idx","rb+");
    }
    else // Se arquivo de dados ou de indice nao existirem
    {
        // Cria o arquivo de indice
        *idx_file = createIDXFile(*arq_dados);
    }

    if(*idx_file)
    {
        printf("Arquivo de indices... OK\n");
    }
    else
    {
        printf("Falha ao abrir arquivo de indice\n");
    }

    // Cria a arvore a partir do arquivo de indice
    createTree(*idx_file);
}


/** Insere o registro passado como argumento no arquivo de dados e cria uma entrada no arquivo de indice para o mesmo */
void cadastrarReg(Registro* reg, FILE* arq_dados, FILE* idx_file)
{
    //Garante que o que será escrito, será escrito no final dos arquivos
    fseek(arq_dados,0,SEEK_END);
    fseek(idx_file,0,SEEK_END);

    long byteoffset = ftell(arq_dados);
    char delimitadorCampo = '|';
    char delimitadorReg = '#';

     // Conta quantos marcadores "#" tem no arquivo de dados para determinar a quantidade total de registros total
    int totalRegs = contaMarcador(arq_dados, '#');

    // Cria o novo id com um número a mais que o total de registros antes de inserir.
    // Ex, se tinham 100 filmes ID ia de 0 a 99, totalRegs retornou 99, então agora o id do novo filme é 100
    reg->id = totalRegs - 1;

    // Printa o registro
    printf("\nRegistro[%d]:\n", reg->id);
    printf("ID: %d\n", reg->id);
    printf("Titulo: %s\n", reg->titulo);
    printf("Descricao: %s\n", reg->descr);
    printf("Producao: %s\n", reg->pais);
    printf("Ano Lancamento: %d\n", reg->ano);
    printf("Duracao: %d\n", reg->duracao);
    printf("Genero: %s\n", reg->genero);
    printf("\n\nInserido com Sucesso!\n");

    // Escreve os campos do registro no arquivo de dados
    fwrite(&reg->id,sizeof(int),1,arq_dados);
    fwrite(&reg->ano,sizeof(int),1,arq_dados);
    fwrite(&reg->duracao,sizeof(int),1,arq_dados);

    fwrite(reg->titulo,strlen(reg->titulo),1,arq_dados);
    fwrite(&delimitadorCampo,sizeof(char),1,arq_dados);

    fwrite(reg->descr,strlen(reg->descr),1,arq_dados);
    fwrite(&delimitadorCampo,sizeof(char),1,arq_dados);

    fwrite(reg->pais,strlen(reg->pais),1,arq_dados);
    fwrite(&delimitadorCampo,sizeof(char),1,arq_dados);

    fwrite(reg->genero,strlen(reg->genero),1,arq_dados);
    fwrite(&delimitadorCampo,sizeof(char),1,arq_dados);
    fwrite(&delimitadorReg,sizeof(char),1,arq_dados);

     // Cria uma nova entrada no arquivo de indice
    fwrite(&reg->id,sizeof(int),1,idx_file);
    fwrite(&byteoffset,sizeof(int),1,idx_file);

    // Incrementa o contador atual de registros
    N_REGS++;

    // Insere a nova key na arvore de indices também
    Key key;
    key.id = reg->id;
    key.byteoffset = byteoffset;

    insertBTree(key);
}

/** Aloca memória para os campos do tipo string, e retorna a string alocada com o texto passado em buffer **/
char* criarCampoFrom(char buffer[])
{
    char* campo = (char*) malloc(strlen(buffer)*sizeof(char));
    strcpy(campo,buffer);
    campo[strlen(campo) - 1] = '\0';
    buffer[0] = '\0'; // Limpa o bufffer
    return campo;
}



/** Lista todos os registros do arquivo de dados */
void listAllRegs(FILE* arq_dados, FILE* idx_file)
{
    int i = 0;
    // Para cada um dos 100 registros,
    for (i = 0; i < N_REGS; i++)
    {
        // Printa um registro na tela
        printReg(i,arq_dados,idx_file);
    }
}

/** Printa um registro com todos os seus campos */
void printReg(int regID, FILE* arq_dados, FILE* idx_file)
{
    // Resgata um registro e armazena num registro temporário
    Registro* reg = getReg(regID,arq_dados, idx_file);

    // Printa o registro
    printf("\nRegistro[%d]:\n", reg->id);
    printf("ID: %d\n", reg->id);
    printf("Titulo: %s\n", reg->titulo);
    printf("Descricao: %s\n", reg->descr);
    printf("Producao: %s\n", reg->pais);
    printf("Ano Lancamento: %d\n", reg->ano);
    printf("Duracao: %d\n", reg->duracao);
    printf("Genero: %s\n", reg->genero);

    // Libera a memória do registro temporário utilizado para printar o registro atual
    freeReg(reg);
}

/** Retorna o registro do arquivo com o ID que foi passado por parametro, utilizando a busca na arvore*/
Registro* getReg(int id, FILE* arq_dados, FILE* idx_file)
{
    Registro* reg = (Registro*)malloc(sizeof(Registro)); // Registro temporario que será retornado preenchido no final da função

    // Utiliza a arvore para localizar a chave como ID informado
    Key* key = search(id);

    // Utilizando o offset contido na chave, o ponteiro do arquivo é posicionado naquele offset
    fseek(arq_dados,key->byteoffset,SEEK_SET);

    // Lê o ID que vem logo em seguida
    int idLido;
    fread(&idLido,sizeof(int),1,arq_dados);

    reg->id = idLido;         // Temorariamente atribui o id aleatorio fornecido ao ID desse registro (será substrituido depois na função carregaRegistros)
    reg->ano = nextInt(arq_dados);        // Lê os primeiros 4 bytes e atribui à variavel int ano
    reg->duracao = nextInt(arq_dados);    // Faz o mesmo pra variavel duração
    reg->titulo = arqTok(arq_dados,'|');  // Copia tudo que vem antes do marcador de fim de campo, na variável titulo
    reg->descr = arqTok(arq_dados,'|');   // Faz o mesmo pros outros campos
    reg->pais = arqTok(arq_dados,'|');
    reg->genero = arqTok(arq_dados,'|');


    return reg;
}

/** Libera a memória utilizada por um registro **/
void freeReg(Registro* reg)
{
    free(reg->titulo);
    free(reg->descr);
    free(reg->pais);
    free(reg->genero);
    free(reg);
}
