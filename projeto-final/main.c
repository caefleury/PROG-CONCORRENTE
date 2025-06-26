#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#define N_ESTUDANTES 10
#define N_BOLSISTAS 3
#define N_FUNCIONARIOS 2
#define N_COZINHEIROS 2
#define N_BANDEJAS 4
#define TEMPO_REPOSICAO 3

int bandejas_disponiveis = N_BANDEJAS;
int bolsistas_esperando = 0;
int estudantes_esperando = 0;
int funcionarios_esperando = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_bolsista = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_estudante = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_funcionario = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_bandeja = PTHREAD_COND_INITIALIZER;

// Simula comida sendo servida
void servir(const char* tipo, int id) {
    printf("%s %d está se servindo...\n", tipo, id);
    sleep(rand()%3 + 2); 
    printf("%s %d terminou de se servir.\n", tipo, id);
}


// Bolsistas
void* bolsista_thread(void* arg) {
    int id = *(int*)arg;
    free(arg);

    for (int i = 0; i < 3; i++) { // num de refeicoes
        pthread_mutex_lock(&mutex);
        bolsistas_esperando++; 

        while (bandejas_disponiveis == 0) { // espera se não há bandeja
            pthread_cond_wait(&cond_bandeja, &mutex); 
        }

        bandejas_disponiveis--; // pega uma bandeja
        bolsistas_esperando--; // saiu da fila de espera
        pthread_mutex_unlock(&mutex);
        servir("Bolsista", id); // se serve
        printf("Bolsista %d está comendo...\n", id); // come 
        sleep(rand()%9 + 10); // simula tempo de alimentação
        printf("Bolsista %d terminou de comer.\n", id);
        pthread_mutex_lock(&mutex);
        bandejas_disponiveis++; // devolve a bandeja

        pthread_cond_broadcast(&cond_bandeja); 

        if (bolsistas_esperando > 0)
            pthread_cond_signal(&cond_bolsista);
        else if (estudantes_esperando > 0)
            pthread_cond_signal(&cond_estudante);
        else
            pthread_cond_signal(&cond_funcionario);

        pthread_mutex_unlock(&mutex);
        sleep(rand()%9 + 10); 
    }

    return NULL;
}

// bolsistas
void* estudante_thread(void* arg) {
    int id = *(int*)arg;
    free(arg);
    for (int i = 0; i < 3; i++) { // Cada estudante faz 3 refeições
        pthread_mutex_lock(&mutex); // entra na região crítica
        estudantes_esperando++; // avisa que está esperando

        // Espera se não há bandeja ou se há bolsista esperando (prioridade)
        while (bandejas_disponiveis == 0 || bolsistas_esperando > 0) {
            // Fica bloqueado até ser sinalizado
            pthread_cond_wait(&cond_estudante, &mutex);
        }

        bandejas_disponiveis--; // pega uma bandeja
        estudantes_esperando--; // saiu da fila de espera
        pthread_mutex_unlock(&mutex); // libera o mutex

        servir("Estudante", id); // se serve
        printf("Estudante %d está comendo...\n", id); // começa a comer
        sleep(rand()%9 + 10); // tempo aleatório comendo
        printf("Estudante %d terminou de comer.\n", id);

        pthread_mutex_lock(&mutex); // volta para devolver a bandeja
        bandejas_disponiveis++; // devolve bandeja
        pthread_cond_broadcast(&cond_bandeja); // avisa todos


        if (bolsistas_esperando > 0)
            pthread_cond_signal(&cond_bolsista); // acorda bolsista
        else if (estudantes_esperando > 0)
            pthread_cond_signal(&cond_estudante); // acorda outro estudante
        else
            pthread_cond_signal(&cond_funcionario); // acorda funcionário

        pthread_mutex_unlock(&mutex); // libera região crítica
        sleep(rand()%9 + 10); // tempo até próxima refeição
    }
    return NULL;
}

// funcionarios
void* funcionario_thread(void* arg) {
    int id = *(int*)arg;
    free(arg);
    for (int i = 0; i < 3; i++) { 
        pthread_mutex_lock(&mutex);
        funcionarios_esperando++; // sinaliza que está esperando

        // Espera se não há bandeja ou se há alguém de prioridade maior esperando
        while (bandejas_disponiveis == 0 || bolsistas_esperando > 0 || estudantes_esperando > 0) {
            // Só acorda quando não há ninguém na frente
            pthread_cond_wait(&cond_funcionario, &mutex);
        }

        bandejas_disponiveis--; // pega bandeja
        funcionarios_esperando--; // saiu da fila de espera
        pthread_mutex_unlock(&mutex); 
        servir("Funcionario", id); // simula servir-se
        printf("Funcionario %d está comendo...\n", id); // começa a comer
        sleep(rand()%9 + 10); // tempo comendo
        printf("Funcionario %d terminou de comer.\n", id);
        pthread_mutex_lock(&mutex); // volta pra devolver bandeja
        bandejas_disponiveis++; // devolve bandeja
        pthread_cond_broadcast(&cond_bandeja); // avisa geral


        if (bolsistas_esperando > 0)
            pthread_cond_signal(&cond_bolsista); // acorda bolsista
        else if (estudantes_esperando > 0)
            pthread_cond_signal(&cond_estudante); // acorda estudante
        else
            pthread_cond_signal(&cond_funcionario); // acorda outro funcionário

        pthread_mutex_unlock(&mutex); // libera região crítica
        sleep(rand()%9 + 10); // tempo até próxima refeição
    }
    return NULL;
}


// cozinheiros
void* cozinheiro_thread(void* arg) {
    int id = *(int*)arg;
    free(arg);
    while (1) {
        sleep(TEMPO_REPOSICAO); // espera o tempo de reposição (simula trabalho)
        pthread_mutex_lock(&mutex); // entra na região crítica para repor
        int falta = N_BANDEJAS - bandejas_disponiveis; // calcula quantas faltam

        // Se há bandejas para repor, repõe todas de uma vez
        if (falta > 0) {
            bandejas_disponiveis = N_BANDEJAS; // repõe tudo
            printf("Cozinheiro %d repôs as bandejas\n", id); // log didático
            // Notifica todos os tipos de agentes que podem estar esperando
            pthread_cond_broadcast(&cond_bandeja);
            pthread_cond_broadcast(&cond_bolsista);
            pthread_cond_broadcast(&cond_estudante);
            pthread_cond_broadcast(&cond_funcionario);
        }
        pthread_mutex_unlock(&mutex); // libera mutex
    }
    return NULL; 
}




int main() {
    srand(time(NULL));
    pthread_t bolsistas[N_BOLSISTAS], estudantes[N_ESTUDANTES], funcionarios[N_FUNCIONARIOS], cozinheiros[N_COZINHEIROS];
    int i;

    
    // cozinheiros
    i = 0;
    while (i < N_COZINHEIROS) {
        int* id = malloc(sizeof(int)); *id = i+1;
        pthread_create(&cozinheiros[i], NULL, cozinheiro_thread, id);
        i++;
    }

    // estudantes
    i = 0;
    while (i < N_ESTUDANTES) {
        int* id = malloc(sizeof(int)); *id = i+1;
        pthread_create(&estudantes[i], NULL, estudante_thread, id);
        i++;
    }

    // funcionários
    i = 0;
    while (i < N_FUNCIONARIOS) {
        int* id = malloc(sizeof(int)); *id = i+1;
        pthread_create(&funcionarios[i], NULL, funcionario_thread, id);
        i++;
    }

    // bolsistas
    i = 0;
    while (i < N_BOLSISTAS) {
        int* id = malloc(sizeof(int)); *id = i+1;
        pthread_create(&bolsistas[i], NULL, bolsista_thread, id);
        // printf("DEBUG: Bolsista %d criado\n", *id);
        i++;
    }

    i = 0;
    while (i < N_BOLSISTAS) { pthread_join(bolsistas[i], NULL); i++; }
    i = 0;
    while (i < N_ESTUDANTES) { pthread_join(estudantes[i], NULL); i++; }
    i = 0;
    while (i < N_FUNCIONARIOS) { pthread_join(funcionarios[i], NULL); i++; }
    

    printf("\nTodos foram servidos\n");
    return 0;
}
