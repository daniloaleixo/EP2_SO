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
  char *ciclista_nesse_metro;
} Posicao;

void *thread_function_ciclista(void *arg);
void inicializa_variaveis_globais();
void imprime_pista();
void imprime_ciclistas_em(Posicao pos);
char time_ciclista(int id_ciclista);

/* variaveis globais */
Posicao *pista;
pthread_mutex_t *semaforo_posicao;
pthread_t *thread_ciclista;

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

  int *pont = malloc_safe(sizeof (int));
  *pont = 5;

  if(pthread_create(&thread_ciclista[0], NULL, thread_function_ciclista, pont)) {
    printf("Erro na criacao da thread.\n");
    abort();
  }
  
  /* Espera todos os ciclistas pararem */
  for(i = 0; i < num_ciclistas; i++)
    pthread_join(thread_ciclista[i], NULL);


  return 0;
}

void *thread_function_ciclista(void *arg) {
  int id_ciclista = *((int *) arg);
  int posicao_anterior, posicao_atual;

  /* Verifica qual e' o time do ciclista */
  if(id_ciclista < n)
    posicao_anterior = LARGADA1;
  else
    posicao_anterior = LARGADA2;

  while(posicao_atual < d) {
    posicao_atual = (posicao_anterior + 1) % d;

    pista[posicao_anterior].ciclista_nesse_metro[id_ciclista] = 0;
    pista[posicao_atual].ciclista_nesse_metro[id_ciclista] = 1;

    posicao_anterior = posicao_atual;
    imprime_pista();
    sleep(1);
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
  semaforo_posicao = malloc_safe(d * sizeof(pthread_mutex_t));
  thread_ciclista = malloc_safe(num_ciclistas * sizeof(pthread_t));

  printf("%d\n", num_ciclistas);
  /* inicializa pista */
  for(i = 0; i < d; i++) {
    pista[i].ciclista_nesse_metro = malloc_safe(num_ciclistas * sizeof(char));
    pthread_mutex_init(&semaforo_posicao[i], NULL);

    for(j = 0; j < num_ciclistas; j++)
      pista[i].ciclista_nesse_metro[j] = 0;
  }

  /* coloca os ciclistas nas largadas */
  for(j = 0; j < n; j++) {
    pista[LARGADA1].ciclista_nesse_metro[j] = TRUE;
    pista[LARGADA2].ciclista_nesse_metro[n + j] = TRUE;
  }
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
