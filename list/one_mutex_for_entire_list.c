#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include "timer.h"

#define MAX_THREADS 7

struct list_node {
    int data;
    struct list_node * next;
};

struct list_node * head_p = NULL;
pthread_mutex_t list_mutex;

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
    }else { 
        return 0; //valor igual a uno de la lista
    }
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


int Insert(int value) {
    pthread_t tid = pthread_self();
    pthread_mutex_lock(&list_mutex);
    printf("Hilo %d bloquea la lista en funcion insert\n" , tid);
    int a = insert(value);
    pthread_mutex_unlock(&list_mutex);
    printf("Hilo %d desloquea la lista en funcion insert\n" , tid);
    return a;
}

int Delete(int value) { 
    pthread_t tid = pthread_self();
    pthread_mutex_lock(&list_mutex);
    printf("Hilo %d bloquea la lista en funcion Delete\n" , tid);
    int a = delete(value);
    pthread_mutex_unlock(&list_mutex);
    printf("Hilo %d desloquea la lista en funcion Delete\n" , tid);
    return a;
}

int Member(int value) {
    pthread_t tid = pthread_self();
    pthread_mutex_lock(&list_mutex);
    printf("Hilo %d bloquea la lista en funcion Member\n" , tid);
    int a = member(value);
    pthread_mutex_unlock(&list_mutex);
    printf("Hilo %d desbloquea la lista en funcion Member\n" , tid);
    return a;
}

void sprint() {
    struct list_node * curr_p = head_p;
    while(curr_p != NULL) {
        printf("%d " , curr_p->data);
        curr_p = curr_p->next;
    }
    printf("\n");
}

int main() {
    double start, finish;
    int list_mutex = 1;
    
    pthread_t threads[MAX_THREADS];

    // if (pthread_mutex_init(&list_mutex, NULL) != 0) {
    //     perror("Error al inicializar el mutex");
    //     return EXIT_FAILURE;
    // }

    GET_TIME(start);
    pthread_create(&threads[0], NULL, (void *(*)(void *))Insert, (void *)5);
    pthread_create(&threads[1], NULL, (void *(*)(void *))Insert, (void *)6);
    pthread_create(&threads[2], NULL, (void *(*)(void *))Insert, (void *)9);
    pthread_create(&threads[3], NULL, (void *(*)(void *))Insert, (void *)2);
    pthread_create(&threads[4], NULL, (void *(*)(void *))Delete, (void *)9);
    pthread_create(&threads[5], NULL, (void *(*)(void *))Insert, (void *)1);
    pthread_create(&threads[6], NULL, (void *(*)(void *))Member, (void *)8);

    for(int i = 0; i < MAX_THREADS ; i++)  {
        pthread_join(threads[i], NULL); //Espera a que termine el hilo 1 , segundo argumento lo que retorna
    }

    GET_TIME(finish);
    printf("Elapsed time = %e seconds\n", finish - start);

    sprint();

}