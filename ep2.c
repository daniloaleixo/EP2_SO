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
float move_ciclista(int id_ciclista, float posicao_atual, float velocidade);

/* variaveis globais */
Posicao *pista;
pthread_mutex_t *semaforo_pista;
pthread_t *thread_ciclista;
pthread_barrier_t barreira1, barreira2;
float *quanto_cada_ciclista_andou;
int num_interacoes = 0;


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

          JUIZ

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


/*


Thread Function

*/
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
  while(num_interacoes < 1){

    if(num_interacoes >= (id_ciclista % n)) {

      posicao_anterior = posicao_atual;
      posicao_atual = (posicao_anterior + 1) % d;

      pista[posicao_anterior].ciclista_nesse_metro[0] = -1;
      pista[posicao_atual].ciclista_nesse_metro[0] = id_ciclista;
      printf("ID_CICLISTA: %d\n", id_ciclista);

      quanto_cada_ciclista_andou[id_ciclista] += 1;
    }


    //printf("%d antes da barreira1\n", id_ciclista);
    pthread_barrier_wait(&barreira1);
    //printf("%d depois da barreira1\n", id_ciclista);
    pthread_barrier_wait(&barreira2);
    //printf("%d depois da barreira2\n", id_ciclista);


    num_interacoes++;
  }

  return NULL;
}





float move_ciclista(int id_ciclista, float posicao_atual, float velocidade) {
  float proxima_posicao = posicao_atual + velocidade;
  int posicao_na_pista = (int) proxima_posicao;



  pthread_mutex_lock(&semaforo_pista[posicao_na_pista]);
  if(posicao_na_pista != posicao_atual) pthread_mutex_lock(&semaforo_pista[(int) posicao_atual]);


  // Caso quando existe alguem ja nesse metro 
  if(pista[posicao_na_pista].ciclista_nesse_metro[0] != -1){  
  
    // Se nao forem do mesmo time
    if(time_ciclista(id_ciclista) !=  time_ciclista(pista[posicao_na_pista].ciclista_nesse_metro[0])){  
      pista[posicao_na_pista].ciclista_nesse_metro[1] = id_ciclista;
      quanto_cada_ciclista_andou[id_ciclista] += velocidade;
    } 
  // Se nao tem ninguem vamos para la
  } else {
    pista[posicao_na_pista].ciclista_nesse_metro[0] = id_ciclista;
    quanto_cada_ciclista_andou[id_ciclista] += velocidade;
  }


  pthread_mutex_unlock(&semaforo_pista[posicao_na_pista]);
  if(posicao_na_pista != posicao_atual) pthread_mutex_unlock(&semaforo_pista[(int) posicao_atual]);

  return proxima_posicao;
}




void inicializa_variaveis_globais() {
  int i, j;
  
  LARGADA1 = 0;
  LARGADA2 = d / 2;

  num_ciclistas = 2 * n;

  /* aloca os vetores que vamos precisar */
  pista = malloc_safe(d * sizeof(Posicao));
  semaforo_pista = malloc_safe(d * sizeof(pthread_mutex_t));
  thread_ciclista = malloc_safe(num_ciclistas * sizeof(pthread_t));
  quanto_cada_ciclista_andou = malloc_safe(num_ciclistas * sizeof(float));


  /* inicializa pista */
  for(i = 0; i < d; i++) {
    pista[i].ciclista_nesse_metro[0] = -1;
    pista[i].ciclista_nesse_metro[1] = -1;
    pthread_mutex_init(&semaforo_pista[i], NULL);
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
  int id0 = pos.ciclista_nesse_metro[0];
  int id1 = pos.ciclista_nesse_metro[1];

  if(id0 != -1)
    printf("%c%d ", time_ciclista(id0), id0 % n);
  if(id1 != -1)
    printf("%c%d ", time_ciclista(id1), id1 % n);

}

char time_ciclista(int id_ciclista) {
  if(id_ciclista < n) return 'A';
  return 'B';
}
