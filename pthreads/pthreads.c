#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h> 

#define NUM_USERS 3 

pthread_t = threads[NUM_USERS];


void *escrever_carta(void *arg){
    int *id = (int *)arg; // converte void* para int*
    printf("Usuário %d começou a escrever uma carta...\n", id);
    sleep(1); // simula o tempo de escrita
    printf("Usuário %d terminou a carta.\n", id);
    return NULL;
}

// A função main com a criação das pthreads

int main(){
    
    for (i = 1; i <= NUM_USUARIOS; i++) {
        id = (int *) malloc(sizeof(int));
        *id = i;  

        pthread_create(&usuarios_thread_id[i], NULL, usuario_thread, id);
    }

    
}