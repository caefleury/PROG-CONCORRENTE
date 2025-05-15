#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define LESTE 0
#define OESTE 1
#define NUM_MACACOS 5
#define NUM_GORILAS 2

int macacos_na_corda = 0;
int direcao_atual = -1; // -1 = sem direção atual
int gorila_na_corda = 0;
int gorilas_esperando = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t macacos_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t gorila_cond = PTHREAD_COND_INITIALIZER;

// Funções de sincronização
void entra_macaco(int direcao) {
    pthread_mutex_lock(&mutex);
    while (gorila_na_corda || gorilas_esperando > 0 || (macacos_na_corda > 0 && direcao != direcao_atual)) {
        pthread_cond_wait(&macacos_cond, &mutex);
    }
    macacos_na_corda++;
    direcao_atual = direcao;
    pthread_mutex_unlock(&mutex);
}

void sai_macaco() {
    pthread_mutex_lock(&mutex);
    macacos_na_corda--;
    if (macacos_na_corda == 0) {
        direcao_atual = -1;
        pthread_cond_broadcast(&gorila_cond);
        pthread_cond_broadcast(&macacos_cond);
    }
    pthread_mutex_unlock(&mutex);
}

void entra_gorila() {
    pthread_mutex_lock(&mutex);
    gorilas_esperando++;
    while (macacos_na_corda > 0 || gorila_na_corda) {
        pthread_cond_wait(&gorila_cond, &mutex);
    }
    gorilas_esperando--;
    gorila_na_corda = 1;
    pthread_mutex_unlock(&mutex);
}

void sai_gorila() {
    pthread_mutex_lock(&mutex);
    gorila_na_corda = 0;
    pthread_cond_broadcast(&macacos_cond);
    pthread_cond_broadcast(&gorila_cond);
    pthread_mutex_unlock(&mutex);
}

// Threads
void *macaco(void *arg) {
    int direcao = *((int *)arg);
    int id = rand() % 1000;
    while (1) {
        entra_macaco(direcao);
        printf("Macaco %d atravessando para %s\n", id, direcao == LESTE ? "LESTE" : "OESTE");
        sleep(1);
        printf("Macaco %d saiu da corda\n", id);
        sai_macaco();
        sleep(rand() % 3 + 1);
    }
    return NULL;
}

void *gorila(void *arg) {
    int id = rand() % 1000;
    while (1) {
        entra_gorila();
        printf("GORILA %d atravessando com exclusividade!\n", id);
        sleep(2);
        printf("GORILA %d saiu da corda\n", id);
        sai_gorila();
        sleep(rand() % 5 + 3);
    }
    return NULL;
}

// Main
int main() {
    pthread_t threads_macacos[NUM_MACACOS];
    pthread_t threads_gorilas[NUM_GORILAS];
    int direcoes[NUM_MACACOS];

    srand(time(NULL));

    // Criar threads de macacos
    for (int i = 0; i < NUM_MACACOS; i++) {
        direcoes[i] = rand() % 2;
        pthread_create(&threads_macacos[i], NULL, macaco, &direcoes[i]);
        sleep(1); // criar em tempos ligeiramente diferentes
    }

    // Criar threads de gorilas
    for (int i = 0; i < NUM_GORILAS; i++) {
        pthread_create(&threads_gorilas[i], NULL, gorila, NULL);
        sleep(2);
    }

    // Join opcional (nunca alcançado neste caso)
    for (int i = 0; i < NUM_MACACOS; i++) pthread_join(threads_macacos[i], NULL);
    for (int i = 0; i < NUM_GORILAS; i++) pthread_join(threads_gorilas[i], NULL);

    return 0;
}
