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
#define CHANCE_CICLISTA_QUEBRAR 0.1
#define NUMERO_VOLTAS 16

typedef struct ciclista {
  char *time;
  int id_ciclista;
}Ciclista;

typedef struct lista_ciclistas_mesmo_lugar {
  Ciclista *ciclista;
  struct lista_ciclistas_mesmo_lugar *prox_ciclista;
} Lista_ciclistas_mesmo_lugar;

typedef struct vetor_pista {
  int posicao;
  Lista_ciclistas_mesmo_lugar *ciclistas_nesse_metro;
}Vetor_Pista;



Ciclista *retira_primeiro_elemento_da_lista(Lista_ciclistas_mesmo_lugar *lista);
void insere_na_lista(Lista_ciclistas_mesmo_lugar *lista, Ciclista *cic);
void imprime_pista();
void imprime_todos_ciclistas();




/* variaveis globais */
Vetor_Pista *pista; 
int d, /* representa o tamanho da pista */
    n, /* representa o numero de ciclistas em cada equipe */
    v_ou_u; /* duas formas de execucao */
Ciclista *ciclistas;
FILE *arquivo_saida;


int main(int argc, char *argv[])
{
  int i;
  char *nome_saida = "saida.txt"; 
  


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
  pista = malloc_safe(d *  sizeof(Vetor_Pista));
  ciclistas = malloc_safe(2 * n * sizeof(Ciclista));
   /* inicializa pista */
  for(i = 0; i < d; i++) { pista[i].posicao = i; pista[i].ciclistas_nesse_metro = NULL; }   
  /* inicializa ciclistas */
  for(i = 0; i < n; i++) { ciclistas[i].id_ciclista = i; ciclistas[i].time = 'A'; } 
  for(i = 0; i < n; i++) { ciclistas[n + i].id_ciclista = i; ciclistas[n + i].time = 'B'; } 


  /* cria aquivo de saida */
  arquivo_saida = cria_arquivo(nome_saida);

  //imprime_todos_ciclistas();
  //imprime_pista()


  return 0;
}



Ciclista *retira_primeiro_elemento_da_lista(Lista_ciclistas_mesmo_lugar *lista)
{
  if(lista == NULL) return NULL;
  Lista_ciclistas_mesmo_lugar *aux = lista;
  lista = lista->prox_ciclista;
  aux->prox_ciclista = NULL;
  return aux->ciclista;
}

void insere_na_lista(Lista_ciclistas_mesmo_lugar *lista, Ciclista *cic)
{
  Lista_ciclistas_mesmo_lugar *aux;

  if(lista != NULL)
    for(aux = lista; aux->prox_ciclista != NULL; aux = aux->prox_ciclista);

  lista = malloc_safe(sizeof(Lista_ciclistas_mesmo_lugar));
  lista->ciclista = cic;
  lista->prox_ciclista = NULL;
}


void imprime_pista()
{
  int i;

  printf("Imprimindo Pista: \n");
  printf("LARGADA1");
  for(i = 0; i < d/2; i++)
  {
    if(0) {}/* mudar para se tem ciclista imprime os ciclistas*/
    else printf("-|");
  }
  printf("LARGADA2");
  for(; i < d; i++)
  {
    if(0) {}/* mudar para se tem ciclista imprime os ciclistas*/
    else printf("-|");
  }
  printf("\n");
}

void imprime_todos_ciclistas()
{
  int i;
  for(i = 0; i < 2 * n; i++)
  {
    printf("ciclista: %c%d\n", ciclistas[i].time, ciclistas[i].id_ciclista);
  }
}