#include "gerador_arquivos.h"

// Define o número inicial de filmes que o arquivo de dados terá
int N_REGS = 100;

/** Escolhe 100 filmes aleatórios dentro todos os filmes presentes no arquivo de filmes,
    atribui IDs aleatórios a esses filmes e escreve no arquivo de dados.
    O ID aleatório gerado é o primeiro a ser escrito no começo de cada registro, seguido dos campos de tamanho fixo como int,
    e em seguida os campos de tamanho variável, com os delimitadores.
    A cada execução, tanto os filmes escolhidos, quanto os IDs atribuídos a esses filmes serão diferentes */
FILE* gerarArqDados()
{
    FILE* arq_filmes = NULL;
    FILE* arq_dados = NULL;

    // Abre o arquivo que contém os filmes para leitura
    arq_filmes = fopen("filmes.txt", "r");

    if (arq_filmes == NULL)
    {
        printf("Arquivo de filmes nao encontrado!\n");
        exit(1);
    }

        // Tenta abrir o arquivo de dados, se existir
        arq_dados = fopen("DADOS.bin", "rb+");


        // Se ele não existir, um novo arquivo é criado
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


        // Gera um vetor com 100 números aleatórios que variam 0 a totalFilmes - 1
        // Será utilizado para ecolher 100 filmes aleatórios dentre o arquivo de filmes
        int* rand_filmes = rand_list(N_REGS,0,totalFilmes - 1);

        // Gera um vetor com 100 números aleatórios que variam 0 a 99
        // Será utilizado para atribuir IDs aleatórios pra os 100 filmes escolhidos
        int* randIDs = rand_list(N_REGS,0,N_REGS - 1);

        char buffer;     // Armazenará 1byte lido do arquivo de filmes
        int inicio = 0;  // Byte que o campo se inicia
        int fim = 0;     // Byte que o campo termina
        int tamanho = 0; // Tamanho to campo em bytes
        int campo = 0;   // Qual é o campo do registro que estamos lendo
        int regCount = 0;// Contador de qual é o registro que estamos lendo

        rewind(arq_dados);

        // Escreve o 1º ID aleatório no começo do arquivo de dados
        fwrite(&randIDs[0],sizeof(int),1,arq_dados);

        printf("Gerando Arquivo de dados...\n");

        // Enquanto não chega no final do arquivo de filmes
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
                // Armazena a posição antes de ter lido o marcador
                fim = ftell(arq_filmes) - 1;

                // Calcula quantos bytes teve entre o último marcador, até esse
                tamanho = fim - inicio;

                // Se forem os 2 primeiros campos do arquivo de filmes, é um número (Ano e duração)
                if (campo < 2)
                {
                    // Armazena os números lidos até antes do marcador, no arquivo de dados, como int
                    copiarUltimoIntLido(arq_filmes,arq_dados,tamanho);
                }
                else // Caso contrário, estamos lendo um campo que é uma string de tamanho variável
                {
                    // Copia a string lida até antes do marcador, no arquivo de dados
                    copiarUltimaStringLida(arq_filmes,arq_dados,tamanho);
                }

                campo++;              //Indica que estamos no próximo campo, pro próximo loop
                fseek(arq_filmes,1,SEEK_CUR); // Pula o caracter |
                inicio = ftell(arq_filmes);   // Armazena a posição atual, que é logo após o caracter |

            }
            // Quando chegar ao fim de um registro
            else if (buffer == '#')
            {
                // Escreve o marcador lido "#" no arquivo de dados
                fwrite(&buffer,1,1,arq_dados);

                // Passa para o proximo registro
                regCount++;

                // Verifica se o próximo registro está dentro do limite
                if(regCount >= N_REGS)
                {
                    // Se nao tiver, os 100 registros foram gerados
                    break;
                }
                // Se ainda estiver faltando gerar registros

                // Escreve o proximo ID aleatório gerado no arquivo de dados
                fwrite(&randIDs[regCount],sizeof(int),1,arq_dados);

                // Avança o ponteiro do arquivo de filmes até o filme aleatógio que foi gerado naquele vetor
                fSeekFilme(rand_filmes[regCount], arq_filmes);

                // Atualiza  a posição que indica o byte que o próximo registro de filme começa
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
    long byteoffset; // Temporario que vai armazernar o byteoffset de onde começa o registro com o ID atual
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

    // Armazena a posição atual do ponteiro do arquivo antes de ler o ID
    byteoffset = ftell(arq_dados);

    // Lê o primeiro ID do arquivo de dados
    fread(&id,sizeof(int),1,arq_dados);

    // Escreve no arquivo de indice o ID e o byteoffset atual desse registro
    fwrite(&id,sizeof(int),1,idx_file);
    fwrite(&byteoffset,sizeof(long),1,idx_file);

    // Pula os 2 próximos ints do arquivo de dados
    fseek(arq_dados,2 * sizeof(int),SEEK_CUR);

    long tamArqDados = getTamArquivo(arq_dados);

    printf("Criando Arquivo de Indices...\n");
    // Enquanto não chegar no final do arquivo de dados
    while(fread(&buffer,sizeof(char),1,arq_dados))
    {
        //Enquanto não chegar no fim do registro
        if(buffer == '#' && (ftell(arq_dados) + 1 < tamArqDados))
        {
            //Salva a posição atual do inicio desse registro
            byteoffset = ftell(arq_dados);

            // Lê o próximo ID do arquivo de dados
            fread(&id,sizeof(int),1,arq_dados);

            // Escreve no arquivo de indice o ID e o byteoffset atual desse registro
            fwrite(&id,sizeof(int),1,idx_file);
            fwrite(&byteoffset,sizeof(long),1,idx_file);

            // Pula os 2 próximos ints do arquivo de dados
            fseek(arq_dados,2 * sizeof(int),SEEK_CUR);
        }
    }
    rewind(idx_file);
    printf("Arquivo de Indice criado com Sucesso!\n");
    return idx_file;
}

/** Avança o ponteiro do arquivo até a posição que começa o primeiro campo do filme n no arquivo de filmes */
void fSeekFilme(int n, FILE* arq_filme)
{
    int marcadorCont = 0; // Contador de quantos marcadores de fim de registro foram encontrados
    char buffer;          // Utilizado para ler um byte do arquivo de filmes
    // Volta ao começo do arquivo de filme
    rewind(arq_filme);

    // Se o registro requisitado é o primeiro
    if(marcadorCont == n)
    {
        // Retorna com o ponteiro do arquivo ainda no começo dele
        return;
    }
    // Enquanto não se chega ao fim do aquivo
    while(fscanf(arq_filme,"%c",&buffer) != EOF)
    {
        // Compara o char lido com o marcador de fim de registro
        if (buffer == '#')
        {
            // Conta quantos registros foram achados até o momento
            marcadorCont++;
        }
        if(marcadorCont == n)
        {
            // Retornando a função aqui, o ponteiro do arquivo está exatamente na posição que começa o registro
            return;
        }
    }
    // Se tudo der certo, não deve chegar aqui
    printf("Filme numero %d nao encontrado no arquivo de dados\n", n);
    exit(1);
}


/**
Copia o ultimo número lido no arquivo de orgigem (string) no arquivo destino, convertido pra int
    Após achar um marcador de divisão de campo no arquivo de filmes,
    "nDigitos" contém quantos bytes existem entre o último marcador | lido e o
    próximo |.Pro exemplo |nDigitos| teria "size" = 4 pois tem 4 dígitos entre
    os 2 marcadores de campo.
    A função volta o ponteiro de arquivo o número de bytes necessário para voltar
    ao começo de onde o número começa, e lê o tudo que tem entre os marcadores, como
    uma string.
    A string depois é convertida para int e é armazenado como int no arquivo de destino

    Argumentos:
        arq_origem: O arquivo de filmes com os campos divididos por | (inclusive os de inteiros, que são strings no momento)
        arq_destino: O arquivo de dados, que armazenará um int com o número convertido do arquivo de origem

    Resultador final:
        O arquivo de dados será inserido com um inteiro lido do arquivo de filmes
 */
void copiarUltimoIntLido(FILE* arq_origem,FILE* arq_destino, int nDigitos)
{
    // Volta o ponteiro do arquivo de filmes para o começo do número a ser convertido pra int
    fseek(arq_origem,-(nDigitos + 1),SEEK_CUR);

    // String que armazenará o número lido
    char strNumero[16];

    // Lê o número do arquivo de filmes, como string
    fread(strNumero,nDigitos,1,arq_origem);

    // Bota um \0 no final da string para evitar lixo
    strNumero[nDigitos] = '\0';

    // Converte a string lida pra int
    int n = atoi(strNumero);

    // Armazena o int convertido no arquivo de dados
    fwrite(&n,sizeof(int),1,arq_destino);
}

/** Copia os "n" bytes anteriores à posição atual do ponteiro de arquivo de origem, no arquivo de destino
    É utilizado para copiar campos de string do arquivo de filmes para o arquivo de dados */
void copiarUltimaStringLida(FILE* arq_origem,FILE* arq_destino, int n)
{
    // Volta o ponteiro do arquivo em n+1 bytes para começar a ler do começo da string a ser copiada
    fseek(arq_origem,-(n + 1),SEEK_CUR);

    char buffer; // Utilizado para ler cada char
    int i = 0;

    // Pelos próximos n chars
    for (i = 0; i < n; i++)
    {
        fread(&buffer,1,1,arq_origem); // Lê um char do arquivo de origem
        fwrite(&buffer,1,1,arq_destino); // Escreve no de destino
    }
    // Escreve o marcador de fim de campo no fim do arquivo de destino
    buffer = '|';
    fwrite(&buffer,1,1,arq_destino);
}
