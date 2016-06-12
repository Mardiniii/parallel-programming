#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define M 3
#define K 2
#define N 3
#define NUM_THREADS 10

int A [M][K] = { {1,4}, {2,5}, {3,6} };
int B [K][N] = { {8,7,6}, {5,4,3} };
int C [M][N];

int main(void) {
   int i,j,k,sum;
   clock_t start = clock();
   for (i = 0; i < M; i++) {
      for (j = 0; j < N; j++) {
         sum = 0;
         for (k = 0; k < K; k++) {
            sum = sum + A[i][k] * B[k][j];
         }
         C[i][j] = sum;
      }
   }
   clock_t end = clock();
   printf("************* \n");
   double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
   printf("The time for the operation was %f \n", time_elapsed_in_seconds);
   printf("************* \n");

   //Print out the resulting matrix
   for(i = 0; i < M; i++) {
      for(j = 0; j < N; j++) {
         printf("%d ", C[i][j]);
      }
      printf("\n");
   }
}