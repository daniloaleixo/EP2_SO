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
#define NUMERO_ENTRADAS 4


int main(int argc, char *argv[])
{

  if(argc >= NUMERO_ENTRADAS)
  {

  }
  else
    printf("Argumentos incorretos. Modo de utilizacao:\n\n"
           "\tep2\n\n");

  return 0;
}