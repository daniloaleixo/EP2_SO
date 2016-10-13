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

typedef struct posicao {
  int ciclista_nesse_metro[2];
} Posicao;

void *thread_function_ciclista(void *arg);
void inicializa_variaveis_globais();
void imprime_pista();
void imprime_ciclistas_em(Posicao pos);
char time_ciclista(int id_ciclista);

/* variaveis globais */
Posicao *pista;
pthread_t *thread_ciclista;
pthread_barrier_t barreira1, barreira2;

int LARGADA1, LARGADA2;
int d, /* representa o tamanho da pista */
    n, /* representa o numero de ciclistas em cada equipe */
    num_ciclistas; /* total de ciclistas */

char v_ou_u; /* forma de execucao */
FILE *arquivo_saida;


int main(int argc, char *argv[])
{
  int i, j;
  char *nome_saida = "saida.txt"; 
  
  /* Pegamos as entradas do programa */
  if(argc >= NUMERO_ENTRADAS + 1)
  {
    d = atoi(argv[1]);
    n = atoi(argv[2]);
    if(argc == NUMERO_ENTRADAS + 2) v_ou_u = argv[3][0];
  }
  else {
    printf("Argumentos incorretos. Modo de utilizacao:\n\n"
           "\tep2\td\tn\t[v|u]\n\n");
    return 0;
  }

  inicializa_variaveis_globais();

  arquivo_saida = cria_arquivo(nome_saida);

  /* Inicializa as threads dos ciclistas */
  int *id_ciclista;
  for(i = 0; i < num_ciclistas; i++) {
    id_ciclista = malloc_safe(sizeof (int));
    *id_ciclista = i;

    if(pthread_create(&thread_ciclista[*id_ciclista], NULL,
                      thread_function_ciclista, id_ciclista)) {
      printf("Erro na criacao da thread.\n");
      abort();
    }
  }


  /* Aqui, o juiz vai ficar olhando pra galera */
  /*

_--------------------------------------------------------_



_--------------------------------------------------------_


  */

  // while(algo) {

  pthread_barrier_wait(&barreira1);
  sleep(2);
  printf("SOU EU, O JUIZ. :D\n");
  pthread_barrier_wait(&barreira2);
  // }

  // corrida_em_andamento = FALSE;

  /* Espera todos os ciclistas pararem */
  for(i = 0; i < num_ciclistas; i++)
    pthread_join(thread_ciclista[i], NULL);

  imprime_pista();

  return 0;
}




void *thread_function_ciclista(void *arg) {
  int id_ciclista = *((int *) arg);
  int posicao_anterior, posicao_atual;
  int i = 0;

  /* Verifica qual e' o time do ciclista */
  if(id_ciclista < n)
    posicao_anterior = LARGADA1;
  else
    posicao_anterior = LARGADA2;

  posicao_atual = (posicao_anterior + 1) % d;
  // while(corrida_em_andamento) {
  while(i < 1){

    posicao_anterior = posicao_atual;
    posicao_atual = (posicao_anterior + 1) % d;

    printf("%d antes da barreira1\n", id_ciclista);
    pthread_barrier_wait(&barreira1);
    printf("%d depois da barreira1\n", id_ciclista);
    pthread_barrier_wait(&barreira2);
    printf("%d depois da barreira2\n", id_ciclista);

    i++;
  }

  return NULL;
}

void inicializa_variaveis_globais() {
  int i, j;
  
  LARGADA1 = 0;
  LARGADA2 = d / 2;

  num_ciclistas = 2 * n;

  /* aloca os vetores que vamos precisar */
  pista = malloc_safe(d * sizeof(Posicao));
  thread_ciclista = malloc_safe(num_ciclistas * sizeof(pthread_t));

  /* inicializa pista */
  for(i = 0; i < d; i++) {
    pista[i].ciclista_nesse_metro[0] = -1;
    pista[i].ciclista_nesse_metro[1] = -1;
  }

  pthread_barrier_init(&barreira1, NULL, num_ciclistas + 1);
  pthread_barrier_init(&barreira2, NULL, num_ciclistas + 1);
}

void imprime_pista()
{
  int i, j, *ids_ciclistas, tamvet;

  printf("Imprimindo Pista: \n");
  printf("LARGADA1 ");
  for(i = 0; i < d/2; i++) {
    imprime_ciclistas_em(pista[i]);
    printf("| ");
  }

  printf(" LARGADA2 ");
  for(; i < d; i++) {
    imprime_ciclistas_em(pista[i]);
    printf("| ");
  }
  printf("\n");
}

void imprime_ciclistas_em(Posicao pos) {
  int i;

  for(i = 0; i < num_ciclistas; i++)
    if(pos.ciclista_nesse_metro[i] == 1)
      printf("%c%d ", time_ciclista(i), i % n);
}

char time_ciclista(int id_ciclista) {
  if(id_ciclista < n) return 'A';
  return 'B';
}
