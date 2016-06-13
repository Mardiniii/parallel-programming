// Sebastian Zapata Mardini
// Ingenieria Electrónica - C.C: 1.017.172.854
// Arquitectura Avanzada de Computadores 2016-1

//Matrix multiplication
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define number of threads
#define NUM_THREADS     4

int matrixSize;
int operation_counter = 0;

struct  results  {
  int  id;
  double  **matrix_c;
};

struct results matrix_result[50];

// Global definitios as document given by the professor
pthread_mutex_t mutex_matrix_multiplication = PTHREAD_MUTEX_INITIALIZER;
pthread_t threads[NUM_THREADS];

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

// This function is edited to show one result matrix from the structure sending the id
void printResult(int result_id){
  int i, j;
  printf("*************************\n");
  printf("Printing result #%d...\n",result_id+1);
  printf("*************************\n");
  for(i=0;i<matrixSize;i++){
    for(j=0;j<matrixSize;j++){
      printf("%lf ", matrix_result[result_id].matrix_c[i][j]);
    }
    printf("\n");
  }
}

void *thread_operation(void *threadid) {
  int i,j,k;
  double sum;
  double **a, **b;
  printf("Start\n");char *fname = "matrices_large.dat"; //Change to matrices_large.dat for performance evaluation
  FILE *fh;

  printf("Start\n");
  fh = fopen(fname, "r");
  //First line indicates how many pairs of matrices there are and the matrix size
  a = allocateMatrix();
  b = allocateMatrix();
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
  for (i = 0; i < matrixSize; i++) {
    for (j = 0; j < matrixSize; j++) {
      sum = 0.0;
      for (k = 0; k < matrixSize; k++) {
        sum = sum + a[i][k] * b[k][j];
      }
      matrix_result[operation_counter].matrix_c[i][j] = sum;
    }
  }


  printf("Multiplying two matrices...\n");
  pthread_mutex_lock( &mutex_matrix_multiplication );
  printf("************* \n");
  printf("Operation Counter value is: %d \n", operation_counter);
  printf("************* \n");
  printResult(operation_counter);
  operation_counter++;
  pthread_mutex_unlock( &mutex_matrix_multiplication );

  printf("Multiplying two matrices...\n");
  // printResult(matrix_result[operation_counter]);
  pthread_exit(NULL);
}

int main(void) {
  int i, j, k;
  long t;
  int nmats, step;
  int operate = 1;
  char *fname = "matrices_large.dat"; //Change to matrices_large.dat for performance evaluation
  FILE *fh;

  printf("Start\n");
  fh = fopen(fname, "r");
  //First line indicates how many pairs of matrices there are and the matrix size
  fscanf(fh, "%d %d\n", &nmats, &matrixSize);

  // Dynamically create matrices of the size needed for the result structure
  for(k=0;k<nmats;k++){
    printf("Creating result matrix #%d\n", k+1);
    matrix_result[k].matrix_c=allocateMatrix();
  }

  clock_t start = clock();
  step = nmats%NUM_THREADS;
  while(operate == 1){
    void *status;
    int rc;
    pthread_attr_t attr;
    /* Initialize and set thread detached attribute */
    pthread_mutex_init(&mutex_matrix_multiplication, NULL);
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    if (operation_counter == (50-step) ) {
      for(t=0; t<step; t++){
        rc = pthread_create(&threads[t], &attr, thread_operation,(void *)t);
        if (rc){
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
        }
      }
      /* Free attribute and wait for the other threads */
      pthread_attr_destroy(&attr);
      // Wait for the work of all the threads with the join and continue
      for(t=0; t<step; t++) {
        rc = pthread_join(threads[t], &status);
        if (rc) {
          printf("ERROR; return code from pthread_join() is %d\n", rc);
          exit(-1);
        }
      }
      operate = 0;
    } else {
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
  }
  clock_t end = clock();
  printf("************* \n");
  double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
  printf("The time for the operation was %f \n", time_elapsed_in_seconds);
  printf("************* \n");
  fclose(fh);

  // Free memory
  // // Free memory
   for(k=0;k<nmats;k++){
    free(*matrix_result[k].matrix_c);
    free(matrix_result[k].matrix_c);
   }
  printf("Done.\n");
  // pthread_mutex_destroy(&mutex_matrix_multiplication);
  // pthread_exit(NULL);
  return 0;
}