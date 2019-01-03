#include "utilitarios.h"


/** Retorna um vetor de ints com n valores aleatórios distintos de range_start até range_end*/
int *rand_list(int n,int range_start, int range_end){

    int *ret;   // Vetor que será retornado
    int num;    // Numero aletório gerado
    int i = 0;  // Contador do for

    // Aloca 100 espaços no vetor
    ret = (int*) malloc (n * sizeof(int));

    // Inicializa o vetor inteiro com um valor que indica vazio (-1)
    for (i = 0; i < n; i++)
    {
        ret[i] = -1;
    }

    // como i vai ser utilizado de novo e comparado com 100, precisamos zerá-lo
    i = 0;

    // Garante que os números aleatórios gerados sejam diferentes a cada execução
    srand(time(NULL));

    // Enquanto 100 números distintos não forem gerados
    while (i != n-1)
    {
        // Gera um número aleatório de range_start a rage_end
        num = (rand() % (range_end + 1) + range_start);

        // Percorre todos os números do vetor
        for (i = 0; i < n; i++)
        {
          // Se número aleatório gerado já está no vetor
          if (ret[i] == num)
                break; // Sai do for e um novo numero será sorteado no próximo loop do while

            // Se chegou até a primeira posição vazia e não deu nenhum repetido
            if (ret[i] == -1){
                ret[i] = num;   // Armazena o número aleatório nessa posição

                break; // Pula a verificação do resto e parte pro próximo sorteio de número aleatório
            }
        }
    }

    return ret;
}
