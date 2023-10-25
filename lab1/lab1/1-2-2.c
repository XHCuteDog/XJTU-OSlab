#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int shared_variable = 0;

sem_t semaphore;

void *increment() {
    for (int i = 0; i < 5000; i++) {
        sem_wait(&semaphore); 
        shared_variable++;
        sem_post(&semaphore);
    }
    return NULL;
}

void *decrement() {
    for (int i = 0; i < 5000; i++) {
        sem_wait(&semaphore);
        shared_variable--;
        sem_post(&semaphore);
    }
    return NULL;
}

int main() {
    pthread_t thread1, thread2;

    if (sem_init(&semaphore, 0, 1) == -1) {
        perror("Failed to initialize semaphore");
        exit(1);
    }

    if (pthread_create(&thread1, NULL, increment, NULL) != 0) {
        perror("Failed to create thread1");
        exit(1);
    }

    if (pthread_create(&thread2, NULL, decrement, NULL) != 0) {
        perror("Failed to create thread2");
        exit(1);
    }

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    sem_destroy(&semaphore);
    printf("Final value of shared variable: %d\n", shared_variable);

    return 0;
}
