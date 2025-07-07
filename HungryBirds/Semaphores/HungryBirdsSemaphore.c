#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

//Problema 4.35 - The Hungry Birds

//Guilherme Sousa Lopes - 535869
//Lucas Rodrigues Aragao - 538390
 
// gcc -pthread -o hungry_semaphore HungryBirdsSemaphore.c
// ./hungry_semaphore <F> <N> <time_sleep>

int F;
int N;
int time_sleep;
int food_count;

sem_t sem_parent, sem_children, sem_food;

void* children(void* arg) {
    int id = *(int*)arg;
    while (1) {
        sem_wait(&sem_food);
        food_count = food_count - 1;
        printf("Filho %d comeu uma porcao. Restam %d.\n", id, food_count);
        if (food_count == 0) {
            printf("Filho %d viu que acabou a comida. Acordando o pai.\n", id);
            sem_post(&sem_parent);
            sem_wait(&sem_children);
            printf("Filho %d viu que o pai repos a comida.\n", id);
        }
        sem_post(&sem_food);
        usleep(time_sleep);
    }
    return NULL;
}

void* parent(void* arg) {
    while (1) {
        sem_wait(&sem_parent);
        food_count = F;
        printf("Pai reabasteceu a comida com %d porcoes.\n", F);
        sem_post(&sem_children);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    F = atoi(argv[1]);
    N = atoi(argv[2]);
    time_sleep = atoi(argv[3]);
    food_count = F;

    pthread_t parent_thread;
    pthread_t *children = malloc(sizeof(pthread_t) * N);
    int *ids = malloc(sizeof(int) * N);

    sem_init(&sem_parent, 0, 0);
    sem_init(&sem_children, 0, 0);
    sem_init(&sem_food, 0, 1);

    pthread_create(&parent_thread, NULL, parent, NULL);
    for (int i = 0; i < N; i++) {
        int *id = malloc(sizeof(int));
        *id = i + 1;
        pthread_create(&children[i], NULL, children, id);
    }

    while(1) {}

    return 0;
}