/* ********************************
    EP1 - Sistemas Operacionais
    Prof. Daniel Batista

    Danilo Aleixo Gomes de Souza
    n USP: 7972370
  
    Carlos Augusto Motta de Lima
    n USP: 7991228

********************************** */

#include "auxiliares.h"

FILE *abre_arquivo(char *nome_arquivo)
{
  FILE *entrada;
  entrada = fopen(nome_arquivo, "r");
  if(entrada == NULL)
  {
    fprintf(stderr, "Nao consegui ler o arquivo!\n");
    exit(-1);
  }

  return entrada;
}

FILE *cria_arquivo(char *nome)
{
  FILE *arq;
  arq = fopen(nome, "wt");

  if(arq == NULL)
  {
    printf("Problemas na CRIACAO do arquivo\n");
    return;
  }
  return arq;
}

/* malloc_safe: testa o ponteiro devolvido por malloc */
void *malloc_safe(size_t n)
{
  void *pt;
  pt = malloc(n);
  if(pt == NULL) {
    printf("ERRO na alocacao de memoria.\n\n");
    exit(-1);
  }
  return pt;
}
