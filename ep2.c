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
#define CHANCE_CICLISTA_QUEBRAR 0.1
#define NUMERO_VOLTAS 16

typedef struct posicao {
  int ciclista_nesse_metro[2];
} Posicao;

typedef struct chegada {
  int id_ciclista;
  int posicao_na_equipe;
  float tempo_chegada;
} Chegada;

void *thread_function_ciclista(void *arg);
void inicializa_variaveis_globais();
void imprime_pista();
void imprime_ciclistas_em(Posicao pos);
char equipe_ciclista(int id_ciclista);
float move_ciclista(int id_ciclista, float posicao_atual, float velocidade);
char quem_venceu(int terceiro_maior_time_A, int terceiro_maior_time_B);
int terceiro_maior_do_time(char equipe);
void insere_chegada_do_ciclista(int id_ciclista);
void imprime_chegada_dos_ciclistas();
int num_chegadas_equipe(char equipe);
float tempo_decorrido();
void reportar_ciclista_quebrado(int id_ciclista, int posicao_atual);
int sorteia_quebrado();

/* variaveis globais */
Posicao *pista;
pthread_mutex_t *semaforo_pista;
int corrida_acabou;

pthread_t *thread_ciclista;
pthread_barrier_t barreira_todos_ciclistas_correram, barreira_apagar_posicao,
                  barreira_coordenador;
float *quanto_cada_ciclista_andou;

Chegada *ordem_chegada;
int topo_ordem_chegada;
int num_chegadas_equipe_A, num_chegadas_equipe_B;
pthread_mutex_t semaforo_ordem_chegada;

int num_interacoes, num_voltas_A, num_voltas_B;
int LARGADA1, LARGADA2;
int d, /* representa o tamanho da pista */
    n, /* representa o numero de ciclistas em cada equipe */
    num_ciclistas; /* total de ciclistas */

char v_ou_u; /* forma de execucao */
FILE *arquivo_saida;

int corrida_acabou = FALSE;
int ciclistas_correndo_A, ciclistas_correndo_B;
int num_ciclistas_quebrados;
int *ciclista_quebrado, quebrado_da_rodada;

int main(int argc, char *argv[]) {
  int i, j, debug = FALSE;
  char *nome_saida = "saida.txt"; 
  srand(time(NULL));
  
  /* Pegamos as entradas do programa */
  if(argc >= 3) {
    d = atoi(argv[1]);
    n = atoi(argv[2]);
    if(argc >= 4) v_ou_u = argv[3][0];
  } else {
    printf("Argumentos incorretos. Modo de utilizacao:\n\n"
           "\tep2\td\tn\t[v|u]\n\n");
    return 0;
  }

  if(argc >= 5) debug = TRUE;

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

/*
_--------------------------------------------------------_

          COORDENADOR

_--------------------------------------------------------_
  */
  int terceiro_maior_time_A, terceiro_maior_time_B;
  char vencedor = FALSE;
  int ultima_volta_com_quebras = -1, n_volta;
  while(!corrida_acabou) {    
    pthread_barrier_wait(&barreira_todos_ciclistas_correram);

    n_volta = num_voltas_A;
    if(num_voltas_B > num_voltas_A) n_volta = num_voltas_B;
    if(n_volta % 4 == 0 && n_volta != ultima_volta_com_quebras) {
      ultima_volta_com_quebras = n_volta;
      quebrado_da_rodada = sorteia_quebrado();
    } else quebrado_da_rodada = -1;

    if(debug) imprime_pista();
    
    terceiro_maior_time_A = terceiro_maior_do_time('A');
    terceiro_maior_time_B = terceiro_maior_do_time('B');

    if(vencedor == FALSE)
      vencedor = quem_venceu(terceiro_maior_time_A, terceiro_maior_time_B);
    
    num_interacoes++;
    
    corrida_acabou = ((num_ciclistas_quebrados + topo_ordem_chegada) >=
                      num_ciclistas);

    pthread_barrier_wait(&barreira_coordenador);
  }

  /* Espera todos os ciclistas pararem */
  for(i = 0; i < num_ciclistas; i++)
    pthread_join(thread_ciclista[i], NULL);

  if(vencedor == 'E')
    fprintf(arquivo_saida, "\nHouve empate!\n\n");
  else
    fprintf(arquivo_saida, "\nA equipe %c venceu!\n\n", vencedor);

  imprime_chegada_dos_ciclistas();

  fclose(arquivo_saida);
  return 0;
}

/*

Thread Function

*/
void *thread_function_ciclista(void *arg) {
  int posicao_anterior, posicao_atual, continuar_correndo = TRUE,
      id_ciclista = *((int *) arg);

  /* Verifica qual e' o time do ciclista */
  if(id_ciclista < n)
    posicao_anterior = LARGADA1;
  else
    posicao_anterior = LARGADA2;

  posicao_atual = (posicao_anterior + 1) % d;
  while(!corrida_acabou) {
    if(id_ciclista == quebrado_da_rodada) {
      continuar_correndo = FALSE;
      ciclista_quebrado[id_ciclista] = TRUE;
      num_ciclistas_quebrados++;
      reportar_ciclista_quebrado(id_ciclista, posicao_atual);
      quanto_cada_ciclista_andou[id_ciclista] = -1;
    }
    if(continuar_correndo) {
      if(num_interacoes - 1 >= (id_ciclista % n)) {
        pthread_mutex_lock(&semaforo_pista[posicao_anterior]);
        pista[posicao_anterior].ciclista_nesse_metro[0] = -1;
        pthread_mutex_unlock(&semaforo_pista[posicao_anterior]);

        pthread_barrier_wait(&barreira_apagar_posicao);

        pthread_mutex_lock(&semaforo_pista[posicao_atual]);
        pista[posicao_atual].ciclista_nesse_metro[0] = id_ciclista;
        pthread_mutex_unlock(&semaforo_pista[posicao_atual]);

        quanto_cada_ciclista_andou[id_ciclista] += 1;
        posicao_anterior = posicao_atual;
        posicao_atual = (posicao_anterior + 1) % d;
      } else
        pthread_barrier_wait(&barreira_apagar_posicao);

      if(quanto_cada_ciclista_andou[id_ciclista] == d * NUMERO_VOLTAS) {
        continuar_correndo = FALSE;
        insere_chegada_do_ciclista(id_ciclista);
      }
      pthread_barrier_wait(&barreira_todos_ciclistas_correram);
      pthread_barrier_wait(&barreira_coordenador);
    } else {
      pthread_barrier_wait(&barreira_apagar_posicao);
      pthread_barrier_wait(&barreira_todos_ciclistas_correram);
      pthread_barrier_wait(&barreira_coordenador);
    }
  }

  return NULL;
}

void inicializa_variaveis_globais() {
  int i, j;
  
  LARGADA1 = 0;
  LARGADA2 = d / 2;

  num_chegadas_equipe_A = num_chegadas_equipe_B = 0;
  num_interacoes = 1;
  num_voltas_A = num_voltas_B = 1;
  corrida_acabou = FALSE;
  topo_ordem_chegada = 0;
  num_ciclistas = 2 * n;
  quebrado_da_rodada = -1;
  num_ciclistas_quebrados = 0;

  ciclistas_correndo_A = ciclistas_correndo_B = n;

  /* aloca os vetores que vamos precisar */
  pista = malloc_safe(d * sizeof(Posicao));
  semaforo_pista = malloc_safe(d * sizeof(pthread_mutex_t));
  thread_ciclista = malloc_safe(num_ciclistas * sizeof(pthread_t));
  quanto_cada_ciclista_andou = malloc_safe(num_ciclistas * sizeof(float));
  ciclista_quebrado = malloc_safe(num_ciclistas * sizeof(int));
  ordem_chegada = malloc_safe(num_ciclistas * sizeof(Chegada));

  pthread_mutex_init(&semaforo_ordem_chegada, NULL);

  /* inicializa pista */
  for(i = 0; i < d; i++) {
    pista[i].ciclista_nesse_metro[0] = -1;
    pista[i].ciclista_nesse_metro[1] = -1;
    pthread_mutex_init(&semaforo_pista[i], NULL);
  }

  for(i = 0; i < num_ciclistas; i++)
    ciclista_quebrado[i] = 0;

  pthread_barrier_init(&barreira_todos_ciclistas_correram, NULL, num_ciclistas + 1);
  pthread_barrier_init(&barreira_coordenador, NULL, num_ciclistas + 1);
  pthread_barrier_init(&barreira_apagar_posicao, NULL, num_ciclistas);
}

char quem_venceu(int terceiro_maior_time_A, int terceiro_maior_time_B) {
  if(terceiro_maior_time_A == d * NUMERO_VOLTAS &&
     terceiro_maior_time_B == d * NUMERO_VOLTAS) return 'E';
  if(terceiro_maior_time_A == d * NUMERO_VOLTAS) return 'A';
  if(terceiro_maior_time_B == d * NUMERO_VOLTAS) return 'B';

  return FALSE;
}

int terceiro_maior_do_time(char equipe) {
  int i, atual, offset, num_voltas,
      maior = -1, segundo_maior = -1, terceiro_maior = -1,
      id_maior = -1, id_segundo = -1, id_terceiro = -1;

  if(equipe == 'A') offset = 0;
  else offset = n;

  for(i = 0; i < n; i++) {
    atual = quanto_cada_ciclista_andou[i + offset];

    if(atual > maior) {
      terceiro_maior = segundo_maior;
      id_terceiro = id_segundo;

      segundo_maior = maior;
      id_segundo = id_maior;

      maior = atual;
      id_maior = i;
    } else if(atual > segundo_maior) {
      terceiro_maior = segundo_maior;
      id_terceiro = id_segundo;

      segundo_maior = atual;
      id_segundo = i;
    } else if(atual > terceiro_maior) {
      terceiro_maior = atual;
      id_terceiro = i;
    }
  }

  if(terceiro_maior % d == 0 && terceiro_maior != 0) {
    if(equipe == 'A' && terceiro_maior / d > num_voltas_A)
      num_voltas = num_voltas_A++;
    else if(equipe == 'B' && terceiro_maior / d > num_voltas_B)
      num_voltas = num_voltas_B++;
    else return terceiro_maior;

    fprintf(arquivo_saida,
            "%7.2fs | Volta n° %2d da equipe %c: 1° %c%d | 2° %c%d | 3° %c%d\n",
            tempo_decorrido(), num_voltas, equipe, equipe, id_maior, equipe,
            id_segundo, equipe, id_terceiro);

  }

  return terceiro_maior;
}

void imprime_pista() {
  int i, j, *ids_ciclistas;

  fprintf(arquivo_saida, "Tempo decorrido %.2fs: \n", tempo_decorrido());
  fprintf(arquivo_saida, "LARGADA1 ");
  for(i = 0; i < d/2; i++) {
    imprime_ciclistas_em(pista[i]);
    fprintf(arquivo_saida, "| ");
  }

  fprintf(arquivo_saida, " LARGADA2 ");
  for(; i < d; i++) {
    imprime_ciclistas_em(pista[i]);
    fprintf(arquivo_saida, "| ");
  }
  fprintf(arquivo_saida, "\n");
}

void imprime_ciclistas_em(Posicao pos) {
  int id0 = pos.ciclista_nesse_metro[0];
  int id1 = pos.ciclista_nesse_metro[1];

  if(id0 != -1)
    fprintf(arquivo_saida, "%c%d ", equipe_ciclista(id0), id0 % n);
  if(id1 != -1)
    fprintf(arquivo_saida, "%c%d ", equipe_ciclista(id1), id1 % n);

}

char equipe_ciclista(int id_ciclista) {
  if(id_ciclista < n) return 'A';
  return 'B';
}

void insere_chegada_do_ciclista(int id_ciclista) {
  float tempo_chegada = tempo_decorrido();
  char equipe = equipe_ciclista(id_ciclista);
  int posicao_na_equipe = num_chegadas_equipe(equipe);

  pthread_mutex_lock(&semaforo_ordem_chegada);
  ordem_chegada[topo_ordem_chegada].id_ciclista = id_ciclista;
  ordem_chegada[topo_ordem_chegada].posicao_na_equipe = posicao_na_equipe;
  ordem_chegada[topo_ordem_chegada].tempo_chegada = tempo_chegada;
  topo_ordem_chegada++;
  pthread_mutex_unlock(&semaforo_ordem_chegada);
}

int num_chegadas_equipe(char equipe) {
  if(equipe == 'A')
    return ++num_chegadas_equipe_A;
  return ++num_chegadas_equipe_B;
}

void imprime_chegada_dos_ciclistas() {
  int i, id_ciclista, posicao_na_equipe;
  char equipe;
  float tempo_chegada;

  fprintf(arquivo_saida, "Ordem de chegada dos ciclistas:\n");
  for(i = 0; i < topo_ordem_chegada; i++) {
    equipe = equipe_ciclista(ordem_chegada[i].id_ciclista);
    id_ciclista = ordem_chegada[i].id_ciclista % n;
    posicao_na_equipe = ordem_chegada[i].posicao_na_equipe;
    tempo_chegada = ordem_chegada[i].tempo_chegada;

    fprintf(arquivo_saida,
            "%7.2fs | %c%d | %dº do seu time\n",
            tempo_chegada, equipe, id_ciclista, posicao_na_equipe);
  }
}

float tempo_decorrido() {
  return num_interacoes * 6.0 / 100;
}

int sorteia_quebrado() {
  int num_azarado, i, n_ciclistas_inteiros_A, n_ciclistas_inteiros_B;
  int *pilha_ciclistas_na_pista, t;


  float sera_que_quebrou = (1.0 * rand() / RAND_MAX);
  if(sera_que_quebrou > 0.1) return -1;

  pilha_ciclistas_na_pista = malloc_safe(num_ciclistas * sizeof(int));
  n_ciclistas_inteiros_A = n_ciclistas_inteiros_B = n;
  t = 0;
  for(i = 0; i < n; i++)
    if(ciclista_quebrado[i]) n_ciclistas_inteiros_A--;
    else pilha_ciclistas_na_pista[t++] = i;
  for(; i < num_ciclistas; i++)
    if(ciclista_quebrado[i]) n_ciclistas_inteiros_B--;
    else pilha_ciclistas_na_pista[t++] = i;

  num_azarado = (int) (1.0 * t * rand() / RAND_MAX);

  i = pilha_ciclistas_na_pista[num_azarado];

  if(ciclista_quebrado[i]) return -1;
  if(equipe_ciclista(i) == 'A' && n_ciclistas_inteiros_A <= 3) return -1;
  if(equipe_ciclista(i) == 'B' && n_ciclistas_inteiros_B <= 3) return -1;

  return i;
}

void reportar_ciclista_quebrado(int id_ciclista, int posicao_atual) {
  fprintf(arquivo_saida,
          "O ciclista %c%d quebrou na posicao %d da volta %d\n",
          equipe_ciclista(id_ciclista), id_ciclista % n, posicao_atual,
          (int) (quanto_cada_ciclista_andou[id_ciclista] / d + 1));
}
