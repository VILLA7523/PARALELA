#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define MAX_THREAD 10

struct list_node {
    int data;
    struct list_node * next;
};

long a = 0;

struct list_node * head_p = NULL;
pthread_rwlock_t rwlock;

int member(int value) {
    struct list_node * curr_p = head_p;
    while(curr_p != NULL && curr_p->data < value) curr_p = curr_p->next;
    if(curr_p == NULL || curr_p->data > value) return 0;
    else return 1;    
}

int insert (int value)
{
    struct list_node * curr_p = *(&head_p);
    struct list_node * prev_p = NULL;
    struct list_node * temp_p;

    while(curr_p != NULL && curr_p->data < value) {
        prev_p = curr_p;
        curr_p = curr_p->next;
    }

    if(curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node));
        temp_p->data = value;
        temp_p->next = curr_p;
        if(prev_p == NULL) *(&head_p) = temp_p;
        else prev_p->next = temp_p;
        return 1;
    }else return 0; //valor igual a uno de la lista
}


int delete(int value) {
    struct list_node * curr_p = *(&head_p);
    struct list_node * prev_p = NULL;

    while(curr_p != NULL && curr_p->data < value) {
        prev_p = curr_p;
        curr_p = curr_p->next;
    }

    if(curr_p != NULL && curr_p->data == value) {
        if(prev_p == NULL) {
            *(&head_p) = curr_p -> next;
            free(curr_p);
        }else{
            prev_p->next = curr_p->next;
            free(curr_p);
        }
        return 1;
    }else {
        return 0;
    }
}

void sprint() {
    struct list_node * curr_p = head_p;
    while(curr_p != NULL) {
        printf("%d " , curr_p->data);
        curr_p = curr_p->next;
    }
}
void* Insert(void * value) {
    int val = *((int * )value);
    pthread_t my_rank = pthread_self();
    pthread_rwlock_wrlock(&rwlock);
    printf("Hilo %d Bloquea insert\n" , my_rank);
    insert(val);
    pthread_rwlock_unlock(&rwlock);
    printf("Hilo %d Desbloquea insert\n" , my_rank);
}

void* Member(void * value) {
    int val = *((int * )value);
    pthread_t my_rank = pthread_self();
    pthread_rwlock_rdlock(&rwlock);
    printf("Hilo %d Bloquea insert\n" , my_rank);
    insert(val);
    pthread_rwlock_unlock(&rwlock);
    printf("Hilo %d Desbloquea insert\n" , my_rank);
}


void* Delete(void * value) {
    int val = *((int * )value);
    pthread_t my_rank = pthread_self();
    pthread_rwlock_wrlock(&rwlock);
    printf("Hilo %d Bloquea insert\n" , my_rank);
    insert(val);
    pthread_rwlock_unlock(&rwlock);
    printf("Hilo %d Desbloquea insert\n" , my_rank);
}


int main(){
    double start, finish;
    pthread_t threads[MAX_THREAD]; 

    if (pthread_rwlock_init(&rwlock, NULL) != 0) {
        printf("Error initializing rwlock\n");
        exit(EXIT_FAILURE);
    }

    GET_TIME(start);
    pthread_create(&threads[0], NULL, (void *(*)(void *))Insert, (void *)5);
    pthread_create(&threads[1], NULL, (void *(*)(void *))Insert, (void *)6);
    pthread_create(&threads[2], NULL, (void *(*)(void *))Insert, (void *)9);
    pthread_create(&threads[3], NULL, (void *(*)(void *))Insert, (void *)2);
    pthread_create(&threads[4], NULL, (void *(*)(void *))Delete, (void *)9);
    pthread_create(&threads[5], NULL, (void *(*)(void *))Insert, (void *)1);
    pthread_create(&threads[6], NULL, (void *(*)(void *))Member, (void *)8);

    for(int i = 0; i < MAX_THREAD ; i++)  {
        pthread_join(threads[i], NULL); //Espera a que termine el hilo 1 , segundo argumento lo que retorna
    }

    GET_TIME(finish);
    printf("Elapsed time = %e seconds\n", finish - start);
    
    pthread_rwlock_destroy(&rwlock);
    sprint();
    return 0; 
} 