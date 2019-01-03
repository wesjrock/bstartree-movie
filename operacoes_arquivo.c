#include "operacoes_arquivo.h"

/** Lê todas as letras encontradas,a partir da posição atual do arquivo até um marcador qualquer, e retorna-as como uma string.
   Funciona de maneira similar ao strtok, mas com arquivo.
    Argumentos:
        arquivo: O arquivo a ser lido, deve estar aberto como leitura e com o ponteiro no local onde deseja-se começar a ler
        marcador: O simbolo que determina o fim do corte que estamos fazendo
    Retorno:
        Uma string com todos os chars até o marcador
**/
char* arqTok(FILE* arquivo, char marcador)
{
    if (arquivo == NULL)
    {
        printf("Arquivo nao aberto, abra o arquivo antes de usar a funcao arqTok(FILE* arquivo, char marcador)\n");
        exit(1);
    }

    char buffer = '\0'; // Utilizado para ler cada byte do arquivo
    char* stringCompleta; // Armazenará a string lida até o marcador

    int bkpPtrArq = ftell(arquivo); // Faz um backup da posição atual do ponteiro do arquivo para poder voltar depois
    int nBytesLidos = 0;              // Contador de quantos bytes foram lidos até o marcador

    // Enquanto não chegar no marcador
    while(buffer != marcador && buffer != EOF)
    {
        // Verifica que chegamos ao fim do arquivo e não encontramos o marcador
        if (buffer== EOF)
        {
            return "";
        }
        fread(&buffer,sizeof(char),1,arquivo); // Lê a proxima letra pra e armazena no buffer
        nBytesLidos++;
    }

    // Aloca o espaço necessário na string para armazenar o que vai ser lido
    stringCompleta = (char*)malloc((nBytesLidos +1) * sizeof(char));

    // Volta o ponteiro do arquivo para a posição original
    fseek(arquivo,bkpPtrArq,SEEK_SET);

    int i = 0; //Utilizado para contar quantos bytes foram lidos até o momento

    // Repete isso por todos os bytes lidos até o marcador
    for ( i = 0; i < nBytesLidos; i++)
    {
        // Lê as letras do arquivo e armazena na string alocada
        fread(&stringCompleta[i],sizeof(char),1,arquivo);
    }
    stringCompleta[i-1] = '\0';

    return stringCompleta;
}

/** Retorna a quantidade total de vezes que um marcador é enontrado no arquivo
    Normalmente utilizado para localizar o marcador # e contar quantos registros o arquivo tem
  **/
int contaMarcador(FILE* arquivo, char marcador)
{
    if (arquivo == NULL)
    {
        printf("Arquivo nao aberto, abra o arquivo antes de usar a funcao contaMarcador(FILE* arquivo, char marcador)\n");
        exit(1);
    }

    int contador = 0;   // Contador de quantas vezes o marcador for encontrado
    char c;             // Utilizado para ler cada byte do arquivo
    int bkpPtrArq = ftell(arquivo); // Backup da posição atual do ponteiro no arquivo

    rewind(arquivo); // Rebubina o arquivo de dados para começar a pesquisa do começo

    // Equanto não chega no final do arquivo
    while(fread(&c,1,1,arquivo))
    {
        // Se o byte lido for o simbilo de termino de registro
        if(c == marcador)
        {
            contador++;
        }
    }

    // Restaura a posição original do ponteiro do arquivo
    fseek(arquivo,bkpPtrArq,SEEK_SET);

    return contador;
}


long getTamArquivo(FILE* fp)
{
    long fileSize = 0;
    long backupFpPos = ftell(fp);
    rewind(fp);
    fseek(fp,0,SEEK_END);
    fileSize = ftell(fp);
    fseek(fp,backupFpPos,SEEK_SET);
    return fileSize;
}

int arqExiste(char* nomeArquivo)
{
    FILE* fp = fopen(nomeArquivo,"rb");
    int existe = 0;
    if (fp != NULL)
    {
        existe = 1;
    }
    fclose(fp);
    return existe;
}

/** Retorna os proximos 4 bytes lidos do arquivo, como int **/
int nextInt(FILE* arquivo)
{
    if (arquivo == NULL)
    {
        printf("Arquivo nao aberto, abra o arquivo antes de usar a funcao nextInt(FILE* arquivo)\n");
        exit(1);
    }

    int bufferInt; // Utilizando para armazernar um inteiro temporario

    // Lê os proximos 4 bytes do arquivo como int e armazna no buffer
    fread(&bufferInt,sizeof(int),1,arquivo);

    return bufferInt;
}

