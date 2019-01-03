#include "gerador_arquivos.h"

// Define o n�mero inicial de filmes que o arquivo de dados ter�
int N_REGS = 100;

/** Escolhe 100 filmes aleat�rios dentro todos os filmes presentes no arquivo de filmes,
    atribui IDs aleat�rios a esses filmes e escreve no arquivo de dados.
    O ID aleat�rio gerado � o primeiro a ser escrito no come�o de cada registro, seguido dos campos de tamanho fixo como int,
    e em seguida os campos de tamanho vari�vel, com os delimitadores.
    A cada execu��o, tanto os filmes escolhidos, quanto os IDs atribu�dos a esses filmes ser�o diferentes */
FILE* gerarArqDados()
{
    FILE* arq_filmes = NULL;
    FILE* arq_dados = NULL;

    // Abre o arquivo que cont�m os filmes para leitura
    arq_filmes = fopen("filmes.txt", "r");

    if (arq_filmes == NULL)
    {
        printf("Arquivo de filmes nao encontrado!\n");
        exit(1);
    }

        // Tenta abrir o arquivo de dados, se existir
        arq_dados = fopen("DADOS.bin", "rb+");


        // Se ele n�o existir, um novo arquivo � criado
        if (arq_dados == NULL)
        {
            arq_dados = fopen("DADOS.bin", "wb+");
            if(arq_dados == NULL)
            {
                printf("Falha ao criar arquivo de dados.\n");
                exit(1);
            }
        }

        // Conta quantos marcadores "#" tem no arquivo de filme para determinar a quantidade total de filmes
        int totalFilmes = contaMarcador(arq_filmes, '#');


        // Gera um vetor com 100 n�meros aleat�rios que variam 0 a totalFilmes - 1
        // Ser� utilizado para ecolher 100 filmes aleat�rios dentre o arquivo de filmes
        int* rand_filmes = rand_list(N_REGS,0,totalFilmes - 1);

        // Gera um vetor com 100 n�meros aleat�rios que variam 0 a 99
        // Ser� utilizado para atribuir IDs aleat�rios pra os 100 filmes escolhidos
        int* randIDs = rand_list(N_REGS,0,N_REGS - 1);

        char buffer;     // Armazenar� 1byte lido do arquivo de filmes
        int inicio = 0;  // Byte que o campo se inicia
        int fim = 0;     // Byte que o campo termina
        int tamanho = 0; // Tamanho to campo em bytes
        int campo = 0;   // Qual � o campo do registro que estamos lendo
        int regCount = 0;// Contador de qual � o registro que estamos lendo

        rewind(arq_dados);

        // Escreve o 1� ID aleat�rio no come�o do arquivo de dados
        fwrite(&randIDs[0],sizeof(int),1,arq_dados);

        printf("Gerando Arquivo de dados...\n");

        // Enquanto n�o chega no final do arquivo de filmes
        while(regCount < N_REGS)
        {
            fscanf(arq_filmes,"%c", &buffer);

            // Se o byte lido for um '\n' ou um caracter fora da tabela ASCII (Mac), ele "pula" esse caractere
            if(buffer < 0 || buffer == '\n')
            {
                inicio = ftell(arq_filmes);
            }
            // Caso seja encontrado um marcador de campo no arquivo de filmes
            else if(buffer == '|')
            {
                // Armazena a posi��o antes de ter lido o marcador
                fim = ftell(arq_filmes) - 1;

                // Calcula quantos bytes teve entre o �ltimo marcador, at� esse
                tamanho = fim - inicio;

                // Se forem os 2 primeiros campos do arquivo de filmes, � um n�mero (Ano e dura��o)
                if (campo < 2)
                {
                    // Armazena os n�meros lidos at� antes do marcador, no arquivo de dados, como int
                    copiarUltimoIntLido(arq_filmes,arq_dados,tamanho);
                }
                else // Caso contr�rio, estamos lendo um campo que � uma string de tamanho vari�vel
                {
                    // Copia a string lida at� antes do marcador, no arquivo de dados
                    copiarUltimaStringLida(arq_filmes,arq_dados,tamanho);
                }

                campo++;              //Indica que estamos no pr�ximo campo, pro pr�ximo loop
                fseek(arq_filmes,1,SEEK_CUR); // Pula o caracter |
                inicio = ftell(arq_filmes);   // Armazena a posi��o atual, que � logo ap�s o caracter |

            }
            // Quando chegar ao fim de um registro
            else if (buffer == '#')
            {
                // Escreve o marcador lido "#" no arquivo de dados
                fwrite(&buffer,1,1,arq_dados);

                // Passa para o proximo registro
                regCount++;

                // Verifica se o pr�ximo registro est� dentro do limite
                if(regCount >= N_REGS)
                {
                    // Se nao tiver, os 100 registros foram gerados
                    break;
                }
                // Se ainda estiver faltando gerar registros

                // Escreve o proximo ID aleat�rio gerado no arquivo de dados
                fwrite(&randIDs[regCount],sizeof(int),1,arq_dados);

                // Avan�a o ponteiro do arquivo de filmes at� o filme aleat�gio que foi gerado naquele vetor
                fSeekFilme(rand_filmes[regCount], arq_filmes);

                // Atualiza  a posi��o que indica o byte que o pr�ximo registro de filme come�a
                inicio = ftell(arq_filmes);

                // Informa que voltaremos a ler e escrever o primeiro campo do novo registro
                campo = 0;
            }
        }

        fclose(arq_filmes);
        printf("Arquivo de dados com 100 filmes aleatorios gerado com sucesso.\n");

    return arq_dados;
}


FILE* createIDXFile(FILE* arq_dados)
{
    int id; // Temporario que vai armazernar o id lido do arquivo de dados
    long byteoffset; // Temporario que vai armazernar o byteoffset de onde come�a o registro com o ID atual
    char buffer;    // Utilizado para ler cada byte to arquivo
    FILE* idx_file = NULL; // Ponteiro para o arquivo de indice

    // Cria o arquivo de indice
    idx_file = fopen("INDICES.idx", "wb+");
    if (idx_file == NULL)
    {
        printf("Falha ao criar o arquivo de indice\n");
        exit(1);
    }

    rewind(arq_dados);

    // Armazena a posi��o atual do ponteiro do arquivo antes de ler o ID
    byteoffset = ftell(arq_dados);

    // L� o primeiro ID do arquivo de dados
    fread(&id,sizeof(int),1,arq_dados);

    // Escreve no arquivo de indice o ID e o byteoffset atual desse registro
    fwrite(&id,sizeof(int),1,idx_file);
    fwrite(&byteoffset,sizeof(long),1,idx_file);

    // Pula os 2 pr�ximos ints do arquivo de dados
    fseek(arq_dados,2 * sizeof(int),SEEK_CUR);

    long tamArqDados = getTamArquivo(arq_dados);

    printf("Criando Arquivo de Indices...\n");
    // Enquanto n�o chegar no final do arquivo de dados
    while(fread(&buffer,sizeof(char),1,arq_dados))
    {
        //Enquanto n�o chegar no fim do registro
        if(buffer == '#' && (ftell(arq_dados) + 1 < tamArqDados))
        {
            //Salva a posi��o atual do inicio desse registro
            byteoffset = ftell(arq_dados);

            // L� o pr�ximo ID do arquivo de dados
            fread(&id,sizeof(int),1,arq_dados);

            // Escreve no arquivo de indice o ID e o byteoffset atual desse registro
            fwrite(&id,sizeof(int),1,idx_file);
            fwrite(&byteoffset,sizeof(long),1,idx_file);

            // Pula os 2 pr�ximos ints do arquivo de dados
            fseek(arq_dados,2 * sizeof(int),SEEK_CUR);
        }
    }
    rewind(idx_file);
    printf("Arquivo de Indice criado com Sucesso!\n");
    return idx_file;
}

/** Avan�a o ponteiro do arquivo at� a posi��o que come�a o primeiro campo do filme n no arquivo de filmes */
void fSeekFilme(int n, FILE* arq_filme)
{
    int marcadorCont = 0; // Contador de quantos marcadores de fim de registro foram encontrados
    char buffer;          // Utilizado para ler um byte do arquivo de filmes
    // Volta ao come�o do arquivo de filme
    rewind(arq_filme);

    // Se o registro requisitado � o primeiro
    if(marcadorCont == n)
    {
        // Retorna com o ponteiro do arquivo ainda no come�o dele
        return;
    }
    // Enquanto n�o se chega ao fim do aquivo
    while(fscanf(arq_filme,"%c",&buffer) != EOF)
    {
        // Compara o char lido com o marcador de fim de registro
        if (buffer == '#')
        {
            // Conta quantos registros foram achados at� o momento
            marcadorCont++;
        }
        if(marcadorCont == n)
        {
            // Retornando a fun��o aqui, o ponteiro do arquivo est� exatamente na posi��o que come�a o registro
            return;
        }
    }
    // Se tudo der certo, n�o deve chegar aqui
    printf("Filme numero %d nao encontrado no arquivo de dados\n", n);
    exit(1);
}


/**
Copia o ultimo n�mero lido no arquivo de orgigem (string) no arquivo destino, convertido pra int
    Ap�s achar um marcador de divis�o de campo no arquivo de filmes,
    "nDigitos" cont�m quantos bytes existem entre o �ltimo marcador | lido e o
    pr�ximo |.Pro exemplo |nDigitos| teria "size" = 4 pois tem 4 d�gitos entre
    os 2 marcadores de campo.
    A fun��o volta o ponteiro de arquivo o n�mero de bytes necess�rio para voltar
    ao come�o de onde o n�mero come�a, e l� o tudo que tem entre os marcadores, como
    uma string.
    A string depois � convertida para int e � armazenado como int no arquivo de destino

    Argumentos:
        arq_origem: O arquivo de filmes com os campos divididos por | (inclusive os de inteiros, que s�o strings no momento)
        arq_destino: O arquivo de dados, que armazenar� um int com o n�mero convertido do arquivo de origem

    Resultador final:
        O arquivo de dados ser� inserido com um inteiro lido do arquivo de filmes
 */
void copiarUltimoIntLido(FILE* arq_origem,FILE* arq_destino, int nDigitos)
{
    // Volta o ponteiro do arquivo de filmes para o come�o do n�mero a ser convertido pra int
    fseek(arq_origem,-(nDigitos + 1),SEEK_CUR);

    // String que armazenar� o n�mero lido
    char strNumero[16];

    // L� o n�mero do arquivo de filmes, como string
    fread(strNumero,nDigitos,1,arq_origem);

    // Bota um \0 no final da string para evitar lixo
    strNumero[nDigitos] = '\0';

    // Converte a string lida pra int
    int n = atoi(strNumero);

    // Armazena o int convertido no arquivo de dados
    fwrite(&n,sizeof(int),1,arq_destino);
}

/** Copia os "n" bytes anteriores � posi��o atual do ponteiro de arquivo de origem, no arquivo de destino
    � utilizado para copiar campos de string do arquivo de filmes para o arquivo de dados */
void copiarUltimaStringLida(FILE* arq_origem,FILE* arq_destino, int n)
{
    // Volta o ponteiro do arquivo em n+1 bytes para come�ar a ler do come�o da string a ser copiada
    fseek(arq_origem,-(n + 1),SEEK_CUR);

    char buffer; // Utilizado para ler cada char
    int i = 0;

    // Pelos pr�ximos n chars
    for (i = 0; i < n; i++)
    {
        fread(&buffer,1,1,arq_origem); // L� um char do arquivo de origem
        fwrite(&buffer,1,1,arq_destino); // Escreve no de destino
    }
    // Escreve o marcador de fim de campo no fim do arquivo de destino
    buffer = '|';
    fwrite(&buffer,1,1,arq_destino);
}
