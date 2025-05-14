#include "stdio.h"
#include "unistd.h"
#include "stdlib.h"
#include "pthread.h"

#define TRUE 1

#define NE 3 //numero de escritores
#define NL 5 //numero de leitores

// chamar mutex
pthread_mutex_t lock_bd = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock_num_leitores = PTHREAD_MUTEX_INITIALIZER;

void * reader(void *arg);
void * writer(void *arg);
void read_data_base(int i);
void use_data_read(int i);
void think_up_data(int i);
void write_data_base(int i);

int num_leitores = 0; // leitores acessando o banco de dados

void* reader(void *arg) {
    int i = *((int *) arg);
    while(TRUE) {        /* repete para sempre */
        
        // leitor chega aqui
        pthread_mutex_lock(&lock_num_leitores)
        num_leitores++;
        pthread_mutex_unlock(&lock_num_leitores)

        if(num_leitores == 1){
            pthread_mutex_lock(&lock_bd); // se for o primeiro leitor ele tranca o bd
        }                                 // se NÃO for, ele não tenta pegar o lock e pula (le do banco)

        read_data_base(i);                  /* acesso aos dados */

        pthread_mutex_lock(&lock_num_leitores)    
        num_leitores--;
        pthread_mutex_unlock(&lock_num_leitores)

        if(num_leitores == 0){
            pthread_mutex_unlock(&lock_bd); // se for o ultimo leitor saindo ele libera
        }

        use_data_read(i);       /* região não crítica */
    }
    pthread_exit(0);
}

void* writer(void *arg) {
    int i = *((int *) arg);
    while(TRUE) {        /* repete para sempre */
        think_up_data(i);       /* região não crítica */
        //lock para escritores
        pthread_mutex_lock(&lock_bd); // bloqueia outros escritores de escreverem ao mesmo tempo
            write_data_base(i);     /* atualiza os dados */
        pthread_mutex_unlock(&lock_bd);
        // unlock

    }
    pthread_exit(0);
}

void read_data_base(int i) {
    printf("Leitor %d está lendo os dados!\n", i);
    sleep(rand() % 5);
}

void use_data_read(int i) {
    printf("Leitor %d está usando os dados lidos!\n", i);
    sleep(rand() % 5);
}

void think_up_data(int i) {
    printf("Escritor %d está pensando no que escrever!\n", i);
    sleep(rand() % 5);
}

void write_data_base(int i) {
    printf("Escritor %d está escrevendo os dados!\n", i);
    sleep(rand() % 5);
}

int main() {

    pthread_t r[NL], w[NE];
    int i;
    int *id;
    /* criando leitores */
    for (i = 0; i < NL ; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&r[i], NULL, reader, (void *) (id));
    }
    /* criando escritores */
    for (i = 0; i< NE; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;
        pthread_create(&w[i], NULL, writer, (void *) (id));
    }

    pthread_join(r[0],NULL);
    return 0;
}