// Sebastian Zapata Mardini
// Ingenieria Electrónica - C.C: 1.017.172.854
// Arquitectura Avanzada de Computadores 2016-1

//Matrix multiplication
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define number of threads
#define NUM_THREADS     2

double **a, **b, **c;
int matrixSize;
int operation_counter;

// Global definitios as document given by the professor
pthread_t threads[NUM_THREADS];
pthread_mutex_t mutex_matrix_multiplication;

double **allocateMatrix() {
  int i;
  double *vals, **temp;

  // allocate space for values
  vals = (double *) malloc (matrixSize * matrixSize * sizeof(double));

  // allocate vector of pointers
  temp = (double **) malloc (matrixSize * sizeof(double*));

  for(i=0; i < matrixSize; i++)
    temp[i] = &(vals[i * matrixSize]);

  return temp;
}

void *thread_operation(void *threadid) {
  int i,j,k;
  double sum;

  for (i = 0; i < matrixSize; i++) {
    for (j = 0; j < matrixSize; j++) {
      sum = 0.0;
      for (k = 0; k < matrixSize; k++) {
        sum = sum + a[i][k] * b[k][j];
      }
      c[i][j] = sum;
    }
  }
  pthread_mutex_lock( &mutex_matrix_multiplication );
  operation_counter++;
  pthread_mutex_unlock( &mutex_matrix_multiplication );
  // pthread_exit((void*) 0);
}

void mm(void) {
  int i,j,k;
  double sum;

  for (i = 0; i < matrixSize; i++) {
    for (j = 0; j < matrixSize; j++) {
      sum = 0.0;
      for (k = 0; k < matrixSize; k++) {
        sum = sum + a[i][k] * b[k][j];
      }
      c[i][j] = sum;
    }
  }
}

void printResult(void){
  int i, j;
  for(i=0;i<matrixSize;i++){
    for(j=0;j<matrixSize;j++){
      printf("%lf ", c[i][j]);
    }
    printf("\n");
  }
}

int main(void) {
  int i, j, k;
  long t;
  int nmats;
  char *fname = "matrices_large.dat"; //Change to matrices_large.dat for performance evaluation
  FILE *fh;

  printf("Start\n");
  fh = fopen(fname, "r");
  //First line indicates how many pairs of matrices there are and the matrix size
  fscanf(fh, "%d %d\n", &nmats, &matrixSize);

  //Dynamically create matrices of the size needed
  a = allocateMatrix();
  b = allocateMatrix();
  c = allocateMatrix();

  printf("Loading %d pairs of square matrices of size %d from %s...\n", nmats, matrixSize, fname);
  clock_t start = clock();

  if( NUM_THREADS == 1 ) {
    // Run the normal matrix multiplication function
    mm();
  // Case 2
  } else if ( (matrixSize % NUM_THREADS) == 0 ) {
    while(operation_counter < nmats){
      for(k=0;k<nmats;k++){
        for(i=0;i<matrixSize;i++){
          for(j=0;j<matrixSize;j++){
            fscanf(fh, "%lf", &a[i][j]);
          }
        }
        for(i=0;i<matrixSize;i++){
          for(j=0;j<matrixSize;j++){
            fscanf(fh, "%lf", &b[i][j]);
          }
        }

        printf("Multiplying two matrices...\n");
        mm();
        printResult();
      }
      // Create threads as documents given with the initial code
      void *status;
      int rc;
      pthread_attr_t attr;
      /* Initialize and set thread detached attribute */
      // pthread_mutex_init(&mutex_matrix_multiplication, NULL);
      pthread_attr_init(&attr);
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

      for(t=0; t<NUM_THREADS; t++){
        rc = pthread_create(&threads[t], &attr, thread_operation,(void *)t);
        if (rc){
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
        }
      }
      /* Free attribute and wait for the other threads */
      pthread_attr_destroy(&attr);
      // Wait for the work of all the threads with the join and continue
      for(t=0; t<NUM_THREADS; t++) {
        rc = pthread_join(threads[t], &status);
        if (rc) {
          printf("ERROR; return code from pthread_join() is %d\n", rc);
          exit(-1);
        }
      }
    }
  // Case 3
  }
  clock_t end = clock();
  printf("************* \n");
  double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
  printf("The time for the operation was %f \n", time_elapsed_in_seconds);
  printf("************* \n");
  fclose(fh);

  // Free memory
  free(*a);
  free(a);
  free(*b);
  free(b);
  free(*c);
  free(c);
  printf("Done.\n");
  // pthread_mutex_destroy(&mutex_matrix_multiplication);
  // pthread_exit(NULL);
  return 0;
}