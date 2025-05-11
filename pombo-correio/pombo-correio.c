// Programação concorrente em C
// Problema do pombo correio: Um pombo correio possui uma mochila para levar cartas escritas por usuários
// de um lugar A para outro lugar B. O pombo leva de A para B apenas quando o número de cartas
// acumuladas em sua mochila chegar a 20.

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Constantes
#define NUM_USUARIOS 10  
#define CAPACIDADE_MOCHILA 20
#define NUM_POMBOS 1

// Variáveis compartilhadas
int cartas_na_mochila = 0;
int pombo_em_a = 1; // 1 = pombo está em A, 0 = pombo está em B

// Mutex e variáveis de condição
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_pombo = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_usuario = PTHREAD_COND_INITIALIZER;

// Declarações de funções
void criar_usuarios();       
void criar_pombos();
void *usuario_thread(void *arg);
void *pombo_thread(void *arg);


int main(int argc, char *argv[]) {    
    pthread_t usuarios[NUM_USUARIOS];
    pthread_t pombos[NUM_POMBOS];
    
    printf("Iniciando simulação do pombo correio\n");
    
    criar_usuarios();
    criar_pombos();
    
    // Aguarda as threads de usuários terminarem
    for (int i = 0; i < NUM_USUARIOS; i++) {
        pthread_join(usuarios[i], NULL);
    }
    
    // Aguarda as threads de pombos terminarem
    for (int i = 0; i < NUM_POMBOS; i++) {
        pthread_join(pombos[i], NULL);
    }
    
    // Libera recursos
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_pombo);
    pthread_cond_destroy(&cond_usuario);
    
    printf("Simulação finalizada\n");
    return 0;
}


void criar_usuarios() {
    int i;
    int *id;
    pthread_t usuarios_thread_id[NUM_USUARIOS];
    
    printf("Criando %d usuários...\n", NUM_USUARIOS);
    
    for (i = 0; i < NUM_USUARIOS; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i + 1;  

        pthread_create(&usuarios_thread_id[i], NULL, usuario_thread, (void *) id);
        printf("Usuário %d criado\n", i+1);
    }
}

void criar_pombos() {
    int i;
    int *id;
    pthread_t pombos_thread_id[NUM_POMBOS];

    printf("Criando %d pombos...\n", NUM_POMBOS);
    
    for (i = 0; i < NUM_POMBOS; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i + 1;  

        pthread_create(&pombos_thread_id[i], NULL, pombo_thread, (void *) id);
        printf("Pombo %d criado\n", i+1);
    }
}

void *usuario_thread(void *arg) {
    int id = *((int *) arg);
    free(arg);
    
    printf("Usuário %d iniciado\n", id);
    
    // Simula usuário escrevendo cartas indefinidamente
    while (1) {
        // Simula o tempo que o usuário leva para escrever uma carta
        sleep(rand() % 5 + 1);
        
        pthread_mutex_lock(&mutex);
        
        // Verifica se o pombo está em A e se há espaço na mochila
        while (!pombo_em_a || cartas_na_mochila >= CAPACIDADE_MOCHILA) {
            printf("Usuário %d aguardando pombo retornar ou espaço na mochila\n", id);
            pthread_cond_wait(&cond_usuario, &mutex);
        }
        
        // Coloca a carta na mochila
        cartas_na_mochila++;
        printf("Usuário %d colocou uma carta na mochila. Total: %d cartas\n", id, cartas_na_mochila);
        
        // Se a mochila estiver cheia, acorda o pombo
        if (cartas_na_mochila == CAPACIDADE_MOCHILA) {
            printf("Mochila cheia! Acordando o pombo...\n");
            pthread_cond_signal(&cond_pombo);
        }
        
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}

void *pombo_thread(void *arg) {
    int id = *((int *) arg);
    free(arg);
    
    printf("Pombo %d iniciado\n", id);
    
    // Pombo trabalha indefinidamente
    while (1) {
        pthread_mutex_lock(&mutex);
        
        // Verifica se há cartas suficientes para fazer a viagem
        while (cartas_na_mochila < CAPACIDADE_MOCHILA) {
            printf("Pombo %d dormindo em A, aguardando mochila encher (%d/%d)\n", 
                   id, cartas_na_mochila, CAPACIDADE_MOCHILA);
            pthread_cond_wait(&cond_pombo, &mutex);
        }
        
        // Viaja de A para B
        printf("Pombo %d viajando de A para B com %d cartas...\n", id, cartas_na_mochila);
        pombo_em_a = 0; // Pombo sai de A
        pthread_mutex_unlock(&mutex);
        
        // Simula o tempo de viagem de A para B
        sleep(3);
        
        pthread_mutex_lock(&mutex);
        
        // Entrega as cartas em B
        printf("Pombo %d chegou em B e está entregando %d cartas...\n", id, cartas_na_mochila);
        sleep(2); // Tempo para entregar as cartas
        cartas_na_mochila = 0; // Esvazia a mochila
        
        // Viaja de B para A
        printf("Pombo %d viajando de B para A com a mochila vazia...\n", id);
        pthread_mutex_unlock(&mutex);
        
        // Simula o tempo de viagem de B para A
        sleep(3);
        
        pthread_mutex_lock(&mutex);
        
        // Chegou em A
        printf("Pombo %d chegou em A e está pronto para receber cartas\n", id);
        pombo_em_a = 1; // Pombo está em A
        
        // Avisa os usuários que o pombo está disponível
        pthread_cond_broadcast(&cond_usuario);
        
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}
