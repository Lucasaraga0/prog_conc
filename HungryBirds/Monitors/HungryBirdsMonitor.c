#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//Problema 4.35 - The Hungry Birds

//Guilherme Sousa Lopes - 535869
//Lucas Rodrigues Aragao - 538390

// gcc -pthread -o hungry_semaphore HungryBirdsSemaphore.c
// ./hungry_semaphore <F> <N> <time_sleep>

typedef struct {
    int F;               
    int countFood;       
    int foodAccess;      
    pthread_mutex_t mtx;
    pthread_cond_t children;
    pthread_cond_t parent;
} Monitor;

Monitor monitor;
int time_sleep;

void monitor_init(int F) {
    monitor.F = F;
    monitor.countFood = F;
    monitor.foodAccess = 1;
    pthread_mutex_init(&monitor.mtx, NULL);
    pthread_cond_init(&monitor.children, NULL);
    pthread_cond_init(&monitor.parent, NULL);
}
    
void get_food(int id) {
    pthread_mutex_lock(&monitor.mtx);

    while (!monitor.foodAccess) {
        pthread_cond_wait(&monitor.children, &monitor.mtx);
    }
    monitor.foodAccess = 0;

    pthread_mutex_unlock(&monitor.mtx);
}

void release_food(int id) {
    pthread_mutex_lock(&monitor.mtx);

    monitor.countFood--;
    printf("Filho %d comeu uma porção. Restam %d.\n", id, monitor.countFood);

    if (monitor.countFood == 0) {
        printf("Filho %d viu que acabou a comida. Acordando o pai.\n", id);
        pthread_cond_signal(&monitor.parent);
        pthread_cond_wait(&monitor.children, &monitor.mtx);
        printf("Filho %d viu que o pai repôs a comida.\n", id);
    }

    monitor.foodAccess = 1;
    pthread_cond_signal(&monitor.children);

    pthread_mutex_unlock(&monitor.mtx);
}

void restock_food() {
    pthread_mutex_lock(&monitor.mtx);

    while (monitor.countFood > 0) {
        pthread_cond_wait(&monitor.parent, &monitor.mtx);
    }

    monitor.countFood = monitor.F;
    printf("Pai reabasteceu a comida com %d porções.\n", monitor.F);
    pthread_cond_broadcast(&monitor.children);

    pthread_mutex_unlock(&monitor.mtx);
}

void* children(void* arg) {
    int id = *(int*)arg;
    while (1) {
        get_food(id);
        release_food(id);
        usleep(time_sleep);
    }
    return NULL;
}

void* parent(void* arg) {
    while (1) {
        restock_food();
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <F> <N> <time_sleep_ms>\n", argv[0]);
        return 1;
    }

    int F = atoi(argv[1]);
    int N = atoi(argv[2]);
    time_sleep = atoi(argv[3]);

    monitor_init(F);

    pthread_t filhos[N];
    pthread_t pai;
    int ids[N];

    for (int i = 0; i < N; i++) {
        ids[i] = i + 1;
        pthread_create(&filhos[i], NULL, children, &ids[i]);
    }

    pthread_create(&pai, NULL, parent, NULL);

    for (int i = 0; i < N; i++) {
        pthread_join(filhos[i], NULL);
    }

    pthread_join(pai, NULL);
    return 0;
}
