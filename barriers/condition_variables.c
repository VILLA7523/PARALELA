#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

#define BARRIER_COUNT 100

int thread_count;
int barrier_thread_count = 0;

pthread_mutex_t barrier_mutex;
pthread_cond_t ok_to_proceed;

void *Thread_work(void* rank) {
#  ifdef DEBUG
   long my_rank = *((long*) rank); 
#  endif
   int i;

   for (i = 0; i < BARRIER_COUNT; i++) {
      pthread_mutex_lock(&barrier_mutex);
      barrier_thread_count++;
      if (barrier_thread_count == thread_count) {
         barrier_thread_count = 0;
#        ifdef DEBUG
         printf("Thread %ld > Signalling other threads in barrier %d\n", my_rank, i);
         fflush(stdout);
#        endif
         pthread_cond_broadcast(&ok_to_proceed);
      } else {
         // Espera a que se desbloquee el mutex y coloca al hilo a dormir
         while (pthread_cond_wait(&ok_to_proceed, &barrier_mutex) != 0);
         // Mutex is relocked at this point.
#        ifdef DEBUG
         printf("Thread %ld > Awakened in barrier %d\n", my_rank, i);
         fflush(stdout);
#        endif
      }
      pthread_mutex_unlock(&barrier_mutex);
#     ifdef DEBUG
      if (my_rank == 0) {
         printf("All threads completed barrier %d\n", i);
         fflush(stdout);
      }
#     endif
   }

   return NULL;
}  /* Thread_work */


/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles; 
   double start, finish;

   thread_count = 1;

   thread_handles = malloc (thread_count*sizeof(pthread_t));
   pthread_mutex_init(&barrier_mutex, NULL);
   pthread_cond_init(&ok_to_proceed, NULL);

   GET_TIME(start);
   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
          Thread_work, (void*)&thread);

   for (thread = 0; thread < thread_count; thread++) {
      pthread_join(thread_handles[thread], NULL);
   }
   GET_TIME(finish);
   printf("Elapsed time = %e seconds\n", finish - start);

   pthread_mutex_destroy(&barrier_mutex);
   pthread_cond_destroy(&ok_to_proceed);
   free(thread_handles);
   return 0;
}  


