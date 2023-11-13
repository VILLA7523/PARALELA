#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_THREAD 9

struct list_node {
    int data;
    struct list_node * next;
    pthread_mutex_t mutex;
};

struct list_node * head_p = NULL;
pthread_mutex_t head_p_mutex;

int Member(int value) {
    struct list_node*temp_p;
    pthread_t tid = pthread_self();
    pthread_mutex_lock(&head_p_mutex);
    printf("Member: Hilo %d bloquea el nodo head (%d) de la lista\n" , tid , head_p->data);
    temp_p = head_p;
    
    while (temp_p != NULL && temp_p->data < value) {
        if (temp_p->next != NULL) { 
            pthread_mutex_lock(&(temp_p->next->mutex));
            printf("Member: Hilo %d bloquea el nodo (%d) de la lista\n" , tid , temp_p->next->data );
        }
        if (temp_p == head_p) {
            pthread_mutex_unlock(&head_p_mutex);
            printf("Member: Hilo %d desbloquea el nodo head (%d) de la lista\n" , tid , head_p->data);
        }
        pthread_mutex_unlock(&(temp_p->mutex));
        printf("Member: Hilo %d desbloquea el nodo (%d) de la lista\n" , tid , temp_p->data );
        temp_p = temp_p->next;
    }
    if (temp_p == NULL || temp_p->data > value) {
        if (temp_p == head_p)  {
            pthread_mutex_unlock(&head_p_mutex);
            printf("Member: Hilo %d desbloquea el nodo head (%d) de la lista\n" , tid , head_p->data);
        }
        if (temp_p != NULL) {
            pthread_mutex_unlock(&(temp_p->mutex));
            printf("Member: Hilo %d desbloquea el nodo (%d) de la lista\n" , tid , temp_p->data );
        }
        return 0;
    } else {
        if (temp_p == head_p) {
            pthread_mutex_unlock(&head_p_mutex);
            printf("Member: Hilo %d desbloquea el nodo head (%d) de la lista\n" , tid , head_p->data);
        }
        pthread_mutex_unlock(&(temp_p->mutex));
        printf("Member: Hilo %d desbloquea el nodo (%d) de la lista\n" , tid , temp_p->data );
        return 1;
    }
} 

int Insert(int value) {
    pthread_t tid = pthread_self();

    pthread_mutex_lock(&head_p_mutex);
    printf("1: Insert: Hilo %d bloquea el nodo head de la lista\n" , tid);

    struct list_node *curr_p = *(&head_p);
    struct list_node *prev_p = NULL;
    struct list_node *temp_p;

    while (curr_p != NULL && curr_p->data < value) {
        if (curr_p->next != NULL) {
            pthread_mutex_lock(&(curr_p->next->mutex));
            printf("2: Insert: Hilo %d bloquea el nodo (%d) de la lista\n", tid , curr_p->next->data);
        }
        if (curr_p == head_p) {
            pthread_mutex_unlock(&head_p_mutex);
            printf("3: Insert: Hilo %d desbloquea el nodo head de la lista\n", tid);
        }
        pthread_mutex_unlock(&(curr_p->mutex));
        printf("4: Insert: Hilo %d desbloquea el nodo (%d) de la lista\n", tid , curr_p->data);
        prev_p = curr_p;
        curr_p = curr_p->next;
    }

    
    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node));
        temp_p->data = value;
        temp_p->next = curr_p;
        printf("6: Insert: Hilo %d inserta dato\n", tid);
        if (prev_p == NULL){
            *(&head_p) = temp_p;
            pthread_mutex_unlock(&head_p_mutex);
            printf("5: Insert: Hilo %d desbloquea el nodo head de la lista\n" , tid);
        }
        else {
            prev_p->next = temp_p;
        }
        
        if (curr_p != NULL) {
            pthread_mutex_unlock(&(curr_p->mutex));
            printf("6: Insert: Hilo %d desbloquea el nodo (%d) de la lista\n", tid , curr_p->data);
        }
        return 1;
    } else {
        if (curr_p == head_p) {
            pthread_mutex_unlock(&head_p_mutex);
            printf("7: Insert: Hilo %d desbloquea el nodo head (%d) de la lista\n", tid , curr_p->data);
        }
        pthread_mutex_unlock(&(curr_p->mutex));
        printf("8: Insert: Hilo %d desbloquea el nodo (%d) de la lista\n", tid , curr_p->data);
        return 0; // valor igual a uno de la lista
    }
}

int Delete(int value) {
    pthread_t tid = pthread_self();
    pthread_mutex_lock(&head_p_mutex);
    printf("1: Delete: Hilo %d bloquea el nodo head de la lista\n" , tid);

    struct list_node *curr_p = *(&head_p);
    struct list_node *prev_p = NULL;

    while (curr_p != NULL && curr_p->data < value) {
        if (curr_p->next != NULL) {
            pthread_mutex_lock(&(curr_p->next->mutex));
            printf("2: Delete: Hilo %d bloquea el nodo (%d) de la lista\n", tid, curr_p->next->data);
        }
        if (curr_p == head_p) {
            pthread_mutex_unlock(&head_p_mutex);
            printf("3: Delete: Hilo %d desbloquea el nodo head (%d) de la lista\n", tid, head_p->data);
        }
        pthread_mutex_unlock(&(curr_p->mutex));
        printf("4: Delete: Hilo %d desbloquea el nodo (%d) de la lista\n", tid, curr_p->data);
        prev_p = curr_p;
        curr_p = curr_p->next;
    }
    if (curr_p != NULL && curr_p->data == value) {
        printf("1: Delete data");
        if (prev_p == NULL) {
            *(&head_p) = curr_p->next;
            pthread_mutex_unlock(&head_p_mutex);
            printf("5: Delete: Hilo %d desbloquea el nodo head de la lista\n" , tid);
            free(curr_p);

        } else {
            prev_p->next = curr_p->next;
            free(curr_p);
            if (curr_p->next != NULL) {
                pthread_mutex_unlock(&(curr_p->next->mutex));
                printf("6: Delete: Hilo %d desbloquea el nodo (%d) de la lista\n", tid, curr_p->next->data);
            }
        }
        return 1;
    } else {
        if (curr_p == head_p) {
            pthread_mutex_unlock(&head_p_mutex);
            printf("7: Delete: Hilo %d desbloquea el nodo head de la lista\n", tid);
        }
        if (curr_p != NULL) {
            pthread_mutex_unlock(&(curr_p->mutex));
            printf("8: Delete: Hilo %d desbloquea el nodo (%d) de la lista\n", tid, curr_p->data);
        }
        return 0;
    }
}

void sprint() {
    printf("LISTA ENLAZADA \n");
    struct list_node * curr_p = head_p;
    while(curr_p != NULL) {
        printf("%d " , curr_p->data);
        curr_p = curr_p->next;
    }
}

int main() {
    double start, finish;
    pthread_t threads[MAX_THREAD]; 

    pthread_mutex_init(&head_p_mutex, NULL);

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

    sprint();
}