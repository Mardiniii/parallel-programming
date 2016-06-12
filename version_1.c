//Matrix multiplication
// Call pthread.h library
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define number of threads
#define NUM_THREADS     10

struct thread_data{
   int thread_id;
   int initial_row;
   int final_row;
};

struct thread_data thread_data_array[NUM_THREADS];

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

void *thread_operation(void *threadarg) {
  int j,k,i,initial_row,final_row;
  struct thread_data *my_data;
  long t_id;
  double sum;
  my_data = (struct thread_data *) threadarg;
  t_id = my_data->thread_id;
  initial_row = my_data->initial_row;
  final_row = my_data->final_row;
  printf("Hola soy el Thread #%ld, multiplicando desde la fila #%d hasta la fila #%d!\n", t_id, initial_row, final_row);
  for (i = initial_row; i < final_row; i++) {
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

void parallel_multiplication(void) {
  int i,j,k,rc,step;
  double sum;
  long t;
  void *status;
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;
  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  // Asignar hilos como filas tenga la matriz
  // numero de filas/numero de threads
  // First case when the number of threads is equal to one
  if( NUM_THREADS == 1 ) {
    mm();
  } else if ( (matrixSize % NUM_THREADS) == 0 ) {
    step = matrixSize/NUM_THREADS;
    for(t=0; t<NUM_THREADS; t++){
      thread_data_array[t].thread_id = t;
      thread_data_array[t].initial_row = t * step;
      thread_data_array[t].final_row = (t+1)*step;
      rc = pthread_create(&threads[t], &attr, thread_operation,(void *) &thread_data_array[t]);
      if (rc){
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
      }
    }
    /* Free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);
    for(t=0; t<NUM_THREADS; t++) {
      rc = pthread_join(threads[t], &status);
      if (rc) {
        printf("ERROR; return code from pthread_join() is %d\n", rc);
        exit(-1);
      }
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
    parallel_multiplication();
    printResult();
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
  return 0;
}