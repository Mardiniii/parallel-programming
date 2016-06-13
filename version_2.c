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
#define NUM_THREADS     4

// Matrix structure to load data before operate
struct  matrix_data  {
  int  id;
  int thread_id;
  double  **matrix_a;
  double  **matrix_b;
  double  **matrix_c;
};

struct matrix_data operation[50];

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

void *parallel_multiplication(void *my_operation) {
  pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
  int i,j,k;
  double sum;
  long tid, operation_id;
  struct matrix_data *my_data;
  my_data = (struct matrix_data *) my_operation;
  operation_id = my_data->id;
  tid = my_data->thread_id;
  pthread_mutex_lock(&mutex1);
  for (i = 0; i < matrixSize; i++) {
    for (j = 0; j < matrixSize; j++) {
      sum = 0.0;
      for (k = 0; k < matrixSize; k++) {
        sum = sum + my_data->matrix_a[i][k] * my_data->matrix_b[i][k];
      }
      operation[operation_id].matrix_c[i][k] = sum;
    }
  }
  pthread_mutex_unlock(&mutex1);
  pthread_exit(NULL);
}

void mm(struct matrix_data operation[], int nmats) {
  int i,j,k,o;
  double sum;
  for (o = 0; o < nmats; i++) {
    for (i = 0; i < matrixSize; i++) {
      for (j = 0; j < matrixSize; j++) {
        sum = 0.0;
        for (k = 0; k < matrixSize; k++) {
          sum = sum + operation[o].matrix_a[i][k] * operation[o].matrix_b[k][j];
        }
        operation[o].matrix_c[i][j] = sum;
      }
    }
  }
}

// void printResult(void){
//   int i, j;
//   for(i=0;i<matrixSize;i++){
//     for(j=0;j<matrixSize;j++){
//       printf("%lf ", c[i][j]);
//     }
//     printf("\n");
//   }
// }

void printResult(struct matrix_data operation[],int operation_id){
  int i, j;
  int operation_number = operation_id;
  printf("***********************\n");
  printf("Imprimiento operacion #%d\n", operation[operation_id].id);
  printf("***********************\n");
  for(i=0;i<matrixSize;i++){
    for(j=0;j<matrixSize;j++){
      printf("%lf ", operation[operation_number].matrix_c[i][j]);
    }
    printf("\n");
  }
}

int main(void) {
  int i, j, k, operation_counter = 0;
  long t;
  int nmats;
  char *fname = "matrices_test.dat"; //Change to matrices_large.dat for performance evaluation
  FILE *fh;

  printf("Start\n");
  fh = fopen(fname, "r");
  //First line indicates how many pairs of matrices there are and the matrix size
  fscanf(fh, "%d %d\n", &nmats, &matrixSize);

  //Dynamically create matrices of the size needed into the structure
  for(k=0;k<nmats;k++){
    operation[k].matrix_a = allocateMatrix();
    operation[k].matrix_b = allocateMatrix();
    operation[k].matrix_c = allocateMatrix();
  }

  // We going to load all the data from the file first on the structure
  // Parallelizing
  printf("Loading %d matrices on structure from %s...\n", nmats, fname);
  clock_t start = clock();
  for(k=0;k<nmats;k++){
    operation[k].id = k;
    printf("Cargando Operación #%d\n", operation[k].id);
    for(i=0;i<matrixSize;i++){
      for(j=0;j<matrixSize;j++){
        fscanf(fh, "%lf", &operation[k].matrix_a[i][j]);
      }
    }
    for(i=0;i<matrixSize;i++){
      for(j=0;j<matrixSize;j++){
        fscanf(fh, "%lf", &operation[k].matrix_b[i][j]);
      }
    }
  }
  printf("Matrices charged on structure\n");
  // printResult(operation,20);

  // After we have the threads we going to evaluate three special cases
  // 1. When the number of threads is equal to one
  // 2. When the matrix size is exactly divisible by the number of threads
  // 3. When the number of matrixes multiplications is not exactly divisible by the number of threads and we need to do special considerations
  // Case 1
  if( NUM_THREADS == 1 ) {
    // Run the normal matrix multiplication function
    mm(operation,nmats);
    printResult(operation,0);
  // Case 2
  } else if ( (matrixSize % NUM_THREADS) == 0 ) {
    // Create threads as documents given with the initial code
    void *status;
    pthread_t threads[NUM_THREADS];
    int rc;
    pthread_attr_t attr;
    /* Initialize and set thread detached attribute */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    while( operation_counter < nmats){
      for(t=0; t<NUM_THREADS; t++){
        operation[operation_counter].thread_id = t;
        rc = pthread_create(&threads[t], &attr, parallel_multiplication,(void *) &operation[operation_counter]);
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
      operation_counter = operation_counter + NUM_THREADS;
    }
    printResult(operation,0);
  // Case 3
  }
  clock_t end = clock();
  printf("************* \n");
  double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
  printf("The time for the operation was %f \n", time_elapsed_in_seconds);
  printf("************* \n");
  fclose(fh);

  // Free memory
  for(k=0;k<nmats;k++){
    free(*operation[k].matrix_a);
    free(operation[k].matrix_a);
    free(*operation[k].matrix_b);
    free(operation[k].matrix_b);
    free(*operation[k].matrix_c);
    free(operation[k].matrix_c);
  }
  printf("Done.\n");
  return 0;
}