// Sebastian Zapata Mardini
// Ingenieria Electrónica - C.C: 1.017.172.854
// Arquitectura Avanzada de Computadores 2016-1

// Matrix multiplication - Versión 1
// Call pthread.h library
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define number of threads
#define NUM_THREADS     2

// Matrix structure to load data before operate
struct  matrix_data  {
  int  id;
  double  **matrix_a;
  double  **matrix_b;
  double  **matrix_c;
};

// Thread Data structure to setup/pass multiple arguments via this structure. Each thread receives a unique instance of the structure.
struct thread_data{
   int thread_id;
};

// Array to management all the threads with specific information for each of them
struct thread_data thread_data_array[NUM_THREADS];

// Matrix definition
double **a, **b, **c;
int matrixSize;

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

void *parallel_multiplication(void *threadarg) {
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
  pthread_exit(NULL);
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
  int i, j, k, t;
  int nmats;
  char *fname = "matrices_test.dat"; //Change to matrices_large.dat for performance evaluation
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

  void *status;
  // Create threads as documents given with the initial code
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;
  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // After we have the threads we going to evaluate three special cases
  // 1. When the number of threads is equal to one
  // 2. When the matrix size is exactly divisible by the number of threads
  // 3. When the number of matrixes multiplications is not exactly divisible by the number of threads and we need to do special considerations
  // Case 1
  if( NUM_THREADS == 1 ) {
    // Run the normal matrix multiplication function
    mm();
  // Case 2
  } else if ( (matrixSize % NUM_THREADS) == 0 ) {
    for(t=0; t<NUM_THREADS; t++){






      rc = pthread_create(&threads[t], &attr, thread_operation,(void *) &thread_data_array[t]);
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
  // Case 3
  }


  // Parallelizing
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


  // Parallelizing
  // for(k=0;k<nmats;k++){
  //   for(i=0;i<matrixSize;i++){
  //     for(j=0;j<matrixSize;j++){
  //       fscanf(fh, "%lf", &a[i][j]);
  //     }
  //   }
  //   for(i=0;i<matrixSize;i++){
  //     for(j=0;j<matrixSize;j++){
  //       fscanf(fh, "%lf", &b[i][j]);
  //     }
  //   }

  //   printf("Multiplying two matrices...\n");
  //   mm();
  //   printResult();
  // }

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
  return 0;
}