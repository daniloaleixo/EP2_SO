/* ********************************
    EP2 - Sistemas Operacionais
    Prof. Daniel Batista

    Danilo Aleixo Gomes de Souza
    n USP: 7972370
  
    Carlos Augusto Motta de Lima
    n USP: 7991228

********************************** */

#include "auxiliares.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <time.h>

#define TRUE 1
#define FALSE 0
#define NUMERO_ENTRADAS 2


typedef struct nome {

}Nome;



int main(int argc, char *argv[])
{
  int d, n, v_ou_u;
  char *nome_saida = "saida.txt"; FILE *arquivo_saida;
  Nome *pista;
  pthread_t *ciclistas;


  /* Pegamos as entradas do programa */
  if(argc >= NUMERO_ENTRADAS + 1)
  {
    d = atoi(argv[1]);
    n = atoi(argv[2]);
    if(argc == NUMERO_ENTRADAS + 2) v_ou_u = atoi(argv[3]);
  }
  else
    printf("Argumentos incorretos. Modo de utilizacao:\n\n"
           "\tep2  d  n   [v|u]\n\n");


  /* aloca os vetores que vamos precisar */
  pista = mallocSafe(d *  sizeof(Nome));
  threads = malloc_safe(n * sizeof(pthread_t));

  /* cria aquivo de saida */
  arquivo_saida = cria_arquivo(nome_saida);





  return 0;
}