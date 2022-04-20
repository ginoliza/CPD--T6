/* Alumno: Gino Liza
 *
 * Programa: Multiplicacion matriz-vector usando pthreads. 
 * Entrada:
 *     none
 *
 * Salida:
 *     y: el vector producto
 *     Imprime el tiempo de cálculo
 *
 * Compilar:  
 *    gcc -g -Wall -o pth_mat_vect_rand_split pth_mat_vect_rand_split.c -lpthread
 * Usar:
 *     ./pth_mat_vect_rand_split <thread_count> <m> <n>
 *
 * Notas:  
 *     1.  El almacenamiento para A, x, y es dinamicamente reservado
 *     2.  El numero de threads(thread_count) deberia ser divisible por m. 
 *		   El programa no verifica esto  
 * 	   3.  Se usa un array 1-dimensional para A y se calcula los subscripts
 *		   usando la formula A[i][j] = A[i*n + j]
 *     4.  Distribucion de A, x e y es logica: las 3 son globalmente compartidas		    
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h"

/* Variables globales */
int     thread_count;
int     m, n;
double* A;
double* x;
double* y;

/* Funciones secuenciales */
void Usage(char* prog_name);
void Gen_matrix(double A[], int m, int n);
void Gen_vector(double x[], int n);

/* Funcion paralela */
void *Pth_mat_vect(void* rank);

/*------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long       thread;
   pthread_t* thread_handles;

   if (argc != 4) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);
   m = strtol(argv[2], NULL, 10);
   n = strtol(argv[3], NULL, 10);

   thread_handles = malloc(thread_count*sizeof(pthread_t));
   A = malloc(m*n*sizeof(double));
   x = malloc(n*sizeof(double));
   y = malloc(m*sizeof(double));
   
   Gen_matrix(A, m, n);

   Gen_vector(x, n);

   for (thread = 0; thread < thread_count; thread++)
      pthread_create(&thread_handles[thread], NULL,
         Pth_mat_vect, (void*) thread);

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);

   free(A);
   free(x);
   free(y);

   return 0;
}  /* main */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Imprime un mensaje mostrando lo que se deberia poner
 *			  en la linea de comando, y termina
 
 * In arg :   prog_name
 */
void Usage (char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count> <m> <n>\n", prog_name);
   exit(0);
}  /* Usage */

/*------------------------------------------------------------------
 * Function: Gen_matrix
 * Purpose:  Genera aleatoriamente los elementos de A
 * In args:  m, n
 * Out arg:  A
 */
void Gen_matrix(double A[], int m, int n) {
   int i, j;
   for (i = 0; i < m; i++)
      for (j = 0; j < n; j++)
         A[i*n+j] = random()/((double) RAND_MAX);
}  /* Gen_matrix */

/*------------------------------------------------------------------
 * Function: Gen_vector
 * Purpose:  Genera aleatoriamente los elementos de x 
 * In arg:   n
 * Out arg:  A
 */
void Gen_vector(double x[], int n) {
   int i;
   for (i = 0; i < n; i++)
      x[i] = random()/((double) RAND_MAX);
}  /* Gen_vector */

/*------------------------------------------------------------------
 * Function:       Pth_mat_vect
 * Purpose:        Multiplica una matriz mxn por un vector columna nx1
 * In arg:         rank
 * Global in vars: A, x, m, n, thread_count
 * Global out var: y
 */
void *Pth_mat_vect(void* rank) {
   long my_rank = (long) rank;
   int i;
   int j; 
   int local_m = m/thread_count; 
   int my_first_row = my_rank*local_m;
   int my_last_row = my_first_row + local_m;
   register int sub = my_first_row*n;
   double start, finish;
   double temp;

   GET_TIME(start);
   for (i = my_first_row; i < my_last_row; i++) {
      y[i] = 0.0;
      for (j = 0; j < n; j++) {
          temp = A[sub++];
          temp *= x[j];
          y[i] += temp;
      }
   }
   GET_TIME(finish);
   printf("Thread %ld > Elapsed time = %e seconds\n", 
      my_rank, finish - start);

   return NULL;
}  /* Pth_mat_vect */