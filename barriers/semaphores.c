
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "timer.h"

#define BARRIER_COUNT 100

int thread_count;
int counter;
sem_t barrier_sems[BARRIER_COUNT];
sem_t count_sem;

void *Thread_work(void* rank) {
    
#  ifdef DEBUG
   long my_rank = *((long *) rank);
#  endif
   int i, j;

   for (i = 0; i < BARRIER_COUNT; i++) {
      sem_wait(&count_sem);
      if (counter == thread_count - 1) {
         counter = 0;
         sem_post(&count_sem);
         for (j = 0; j < thread_count-1; j++)
            sem_post(&barrier_sems[i]);
      } else {
         counter++;
         sem_post(&count_sem);
         sem_wait(&barrier_sems[i]);
      }
#     ifdef DEBUG
      //printf("rank: %ld\n" , my_rank);
      if (my_rank == 0) {
         printf("All threads completed barrier %d\n", i);
         fflush(stdout);
      }
#     endif
   }

   return NULL;
} 


int main(int argc, char* argv[]) {
   long       thread, i;
   pthread_t* thread_handles; 
   double start, finish;

   thread_count = 8;

   //reserva la memoria para thread_count hilos
   thread_handles = malloc (thread_count*sizeof(pthread_t));

   //Inicialisando los semaforos para las n BARRIER_COUNT que usaremos
   for (i = 0; i < BARRIER_COUNT; i++)
      sem_init(&barrier_sems[i], 0, 0); //Cada semaforo se inicializa en 0
   sem_init(&count_sem, 0, 1);

   //Se halla el tiempo
   GET_TIME(start);

   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], (pthread_attr_t*) NULL, Thread_work, (void*)&thread);
   for (thread = 0; thread < thread_count; thread++) pthread_join(thread_handles[thread], NULL);
   
   GET_TIME(finish);
   printf("Elapsed time = %e seconds\n", finish - start);

   //Se libera cualquier recurso asociado al semáforo, y es importante llamarla cuando el semáforo ya no se necesite.
   sem_destroy(&count_sem);
   for (i = 0; i < BARRIER_COUNT; i++)
      sem_destroy(&barrier_sems[i]);
    
    //libera memria de los n hilos reservados
   free(thread_handles);
   return 0;

}  


