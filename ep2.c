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


int main(int argc, char *argv[])
{

  if(argc >= 4)
  {
    depurar = (argc == 5 && strcmp(argv[4], "d") == 0);

    /* le o arquivo de trace da entrada e coloca todos os processos numa
       lista encadeada (lista_processos) */
    lista_processos = interpreta_entrada(argv[2]);

    nome_saida = argv[3];
    arquivo_saida = cria_arquivo(nome_saida);

    /* pega o numero de processadores e aloca um vetor de inteiros para
       guardar o estado atual (EM_USO ou LIVRE) de cada processador,
       alem de alocar um vetor de semaforos (um por processador) e um vetor de
       threads (uma por processador) */
    num_procs = get_nprocs();
    estado_processador = malloc_safe(num_procs * sizeof(int));
    for(i = 0; i < num_procs; i++)
      estado_processador[i] = LIVRE;
    semaforo_processador = malloc_safe(num_procs * sizeof(pthread_mutex_t));
    threads = malloc_safe(num_procs * sizeof(pthread_t));

    /* escolhe o metodo de escalonamento */
    switch(atoi(argv[1]))
    {
      case 1:
        first_come_first_served();
        break;

      case 2:
        shortest_remaining_time_next(lista_processos);
        break;

      case 3:
        escalonamento_multiplas_filas(lista_processos);
        break;
    }
  }
  else
    printf("Argumentos incorretos. Modo de utilizacao:\n\n"
           "\tep1 <numero do Metodo de Escalonamento> <arquivo de entrada> "
           "<arquivo de saida>\n\n");

  /* espera ate que todos os processadores estejam LIVREs (ou seja, espera
     que todas as threads terminem a execucao) */
  if(num_procs != 0)
  {  
    /*for(i = 0; i < num_procs; i++){
      if(estado_processador[i] == EM_USO) i = 0;
	/* DEPURACAO * printf("estou em uso\n");*
	}*/
    for(i = 0; i < num_procs; i++)
    	pthread_join(threads[i], NULL);
    
    fprintf(arquivo_saida, "\n");
    fclose(arquivo_saida);

    if(depurar)
      fprintf(stderr, "Quantidade de mudanças de contexto: %d\n",
              qtde_mudancas_contexto);
  }
  /* DEPURACAO */ printf("%d\n", contador_deadlines_estourados);

  return 0;
}

void *thread_function(void *arg)
{
  float t0_processo = tempo_decorrido(),
        tempo_decorrido_processo = 0;
  int i;
  Processo *proc = (Processo *) arg;

  if(depurar)
    fprintf(stderr, "%8.4fs | O processo %s esta usando a CPU: %d\n",
            tempo_decorrido(), proc->nome, proc->processador);

  while(tempo_decorrido_processo < proc->dt)
  {
    tempo_decorrido_processo = tempo_decorrido() - t0_processo;
    i = (i + 1) % 100000;
  }

  pthread_mutex_lock(&semaforo_arq_saida);
  fprintf(arquivo_saida, "%s %f %f\n",
          proc->nome, tempo_decorrido(), tempo_decorrido_processo);
  if(depurar)
    fprintf(stderr, "%8.4fs | O processo %s terminou e esta na linha %d do "
                    "arquivo de saida\n", tempo_decorrido(), proc->nome,
                    ++linha_arquivo_saida);
  pthread_mutex_unlock(&semaforo_arq_saida);

  if(tempo_decorrido() > proc->deadline) contador_deadlines_estourados++;

  pthread_mutex_lock(&semaforo_processador[proc->processador]);
  estado_processador[proc->processador] = LIVRE;
  pthread_mutex_unlock(&semaforo_processador[proc->processador]);

  if(depurar)
    fprintf(stderr, "%8.4fs | O processo %s esta deixando a CPU: %d\n",
            tempo_decorrido(), proc->nome, proc->processador);
  return NULL;
}

/* *****************************************

                funcoes de escalonamento

  ***************************************** */

void first_come_first_served()
{
  Processo *processo_atual;
  int i;

  /* pega da lista o primeiro processo */
  pthread_mutex_lock(&semaforo_lista_processos);
  processo_atual = retira_primeiro_elemento_da_lista();
  pthread_mutex_unlock(&semaforo_lista_processos);

  while(processo_atual != NULL)
  {
    if(depurar)
      fprintf(stderr, "%8.4fs | Chegada do processo %s - linha %d no trace\n",
              tempo_decorrido(), processo_atual->nome,
              processo_atual->linha_no_arquivo_trace);

    /* Espera até que o processo chegue (t >= t0) */
    while(tempo_decorrido() < processo_atual->t0) usleep(100);

    /* procura um processador disponivel */
    for(i = 0; estado_processador[i] != LIVRE; i = (i + 1) % num_procs);
       
    /* marca o processador encontrado como EM_USO e, no processo a ser rodado,
       o processador que ele está usando como i+1 */
    pthread_mutex_lock(&semaforo_processador[i]);
    estado_processador[i] = EM_USO;
    processo_atual->processador = i + 1;
    pthread_mutex_unlock(&semaforo_processador[i]);

    /* cria uma thread para o processo_atual e roda ela durante
       processo_atual->dt segundos (com consumo de CPU) */
    if(pthread_create(&threads[i], NULL, thread_function, processo_atual))
    {
      printf("Erro na criacao da thread.\n");
      abort();
    }

    /* tira o proximo processo da lista */
    pthread_mutex_lock(&semaforo_lista_processos);
    processo_atual = retira_primeiro_elemento_da_lista();
    pthread_mutex_unlock(&semaforo_lista_processos);
  }
}

void shortest_remaining_time_next(Processo *lista)
{
  merge_sort(&lista, 3);
  /*return lista;*/
}

void escalonamento_multiplas_filas(Processo *lista)
{
  merge_sort(&lista, 5);
  /*return lista;*/
}

/* *****************************************

                funcoes auxiliares 

  ***************************************** */

Processo *retira_primeiro_elemento_da_lista()
{
  if(lista_processos == NULL) return NULL;
  Processo *elemento = lista_processos;
  lista_processos = lista_processos->prox;
  elemento->prox = NULL;
  return elemento;
}

Processo *ordenar_metodo2(Processo *lista)
{
  merge_sort(&lista, 2);
  return lista;
}

/* imprime todos os processos da lista encadeada de processos */
void imprime_todos_procs()
{
  Processo *proc;
  for(proc = lista_processos; proc != NULL; proc = proc->prox)
    printf("%s\n", proc->nome);
}

/* le o arquivo de entrada (trace) e devolve uma lista ligada de processos */
Processo *interpreta_entrada(char *nome_arquivo)
{
  Processo *lista, *proc, *novo_proc;
  FILE *arquivo_entrada = abre_arquivo(nome_arquivo);
  double t0;
  int i = 1;

  proc = malloc_safe(sizeof(Processo));
  lista = proc;

  proc->nome = malloc_safe(64 * sizeof(char));
  proc->processador = -1;
  proc->linha_no_arquivo_trace = i++;
  fscanf(arquivo_entrada,"%lf", &(proc->t0));
  fscanf(arquivo_entrada,"%s", proc->nome);
  fscanf(arquivo_entrada,"%lf", &(proc->dt));
  fscanf(arquivo_entrada,"%lf", &(proc->deadline));

  while(fscanf(arquivo_entrada, "%lf", &t0) == 1)
  {
    novo_proc = malloc_safe(sizeof(Processo));
    novo_proc->nome = malloc_safe(64 * sizeof(char));
    novo_proc->t0 = t0;
    novo_proc->processador = -1;
    novo_proc->linha_no_arquivo_trace = i++;
    fscanf(arquivo_entrada,"%s", novo_proc->nome);
    fscanf(arquivo_entrada,"%lf", &(novo_proc->dt));
    fscanf(arquivo_entrada,"%lf", &(novo_proc->deadline));
    proc->prox = novo_proc;
    proc = novo_proc;
  }

  return lista;
}

/* MERGE SORT
   FONTE: http://www.geeksforgeeks.org/merge-sort-for-linked-list/
*/
 
/* sorts the linked list by changing next pointers (not data) */
void merge_sort(Processo** headRef, int mode)
{
  Processo* head = *headRef;
  Processo* a;
  Processo* b;

  /* Base case -- length 0 or 1 */
  if((head == NULL) || (head->prox == NULL)) return;
 
  /* Split head into 'a' and 'b' sublists */
  front_back_split(head, &a, &b);
 
  /* Recursively sort the sublists */
  merge_sort(&a, mode);
  merge_sort(&b, mode);
 
  /* answer = merge the two sorted lists together */
  *headRef = sorted_merge(a, b, mode);
}
 
/* See http://geeksforgeeks.org/?p=3622 for details of this
   function */
Processo* sorted_merge(Processo* a, Processo* b, int mode)
{
  Processo* result = NULL;
 
  /* Base cases */
  if(a == NULL)
     return(b);
  else if(b==NULL)
     return(a);
 
  /* Pick either a or b, and recur */
  if(compare(a, b, mode))
  {
     result = a;
     result->prox = sorted_merge(a->prox, b, mode);
  }
  else
  {
     result = b;
     result->prox = sorted_merge(a, b->prox, mode);
  }
  return(result);
}
 
/* UTILITY FUNCTIONS */
/* Split the nodes of the given list into front and back halves,
     and return the two lists using the reference parameters.
     If the length is odd, the extra node should go in the front list.
     Uses the fast/slow pointer strategy.  */
void front_back_split(Processo* source,
          Processo** frontRef, Processo** backRef)
{
  Processo* fast;
  Processo* slow;
  if (source==NULL || source->prox==NULL)
  {
    /* length < 2 cases */
    *frontRef = source;
    *backRef = NULL;
  }
  else
  {
    slow = source;
    fast = source->prox;
 
    /* Advance 'fast' two nodes, and advance 'slow' one node */
    while (fast != NULL)
    {
      fast = fast->prox;
      if (fast != NULL)
      {
        slow = slow->prox;
        fast = fast->prox;
      }
    }
 
    /* 'slow' is before the midpoint in the list, so split it in two
      at that point. */
    *frontRef = source;
    *backRef = slow->prox;
    slow->prox = NULL;
  }
}

int compare(Processo *a, Processo *b, int mode)
{
  if(mode == 2)
    return (a->dt <= b->dt ? 1 : 0);
  else if(mode == 3)
    return (a->deadline <= b->deadline ? 1 : 0);
}
