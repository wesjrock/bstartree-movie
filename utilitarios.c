#include "utilitarios.h"


/** Retorna um vetor de ints com n valores aleat�rios distintos de range_start at� range_end*/
int *rand_list(int n,int range_start, int range_end){

    int *ret;   // Vetor que ser� retornado
    int num;    // Numero alet�rio gerado
    int i = 0;  // Contador do for

    // Aloca 100 espa�os no vetor
    ret = (int*) malloc (n * sizeof(int));

    // Inicializa o vetor inteiro com um valor que indica vazio (-1)
    for (i = 0; i < n; i++)
    {
        ret[i] = -1;
    }

    // como i vai ser utilizado de novo e comparado com 100, precisamos zer�-lo
    i = 0;

    // Garante que os n�meros aleat�rios gerados sejam diferentes a cada execu��o
    srand(time(NULL));

    // Enquanto 100 n�meros distintos n�o forem gerados
    while (i != n-1)
    {
        // Gera um n�mero aleat�rio de range_start a rage_end
        num = (rand() % (range_end + 1) + range_start);

        // Percorre todos os n�meros do vetor
        for (i = 0; i < n; i++)
        {
          // Se n�mero aleat�rio gerado j� est� no vetor
          if (ret[i] == num)
                break; // Sai do for e um novo numero ser� sorteado no pr�ximo loop do while

            // Se chegou at� a primeira posi��o vazia e n�o deu nenhum repetido
            if (ret[i] == -1){
                ret[i] = num;   // Armazena o n�mero aleat�rio nessa posi��o

                break; // Pula a verifica��o do resto e parte pro pr�ximo sorteio de n�mero aleat�rio
            }
        }
    }

    return ret;
}
