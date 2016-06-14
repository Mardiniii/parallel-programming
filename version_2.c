//Matrix multiplication


// Matrix multiplication - Versión 1
// Call pthread.h library
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define number of threads
#define NUM_THREADS     4

// Thread Data structure to setup/pass multiple arguments via this structure. Each thread receives a unique instance of the structure.
struct thread_data{
   int thread_id;
   double **matrix_a;
   double **matrix_b;
   double **matrix_c;
};

// Array to management all the threads with specific information for each of them
struct thread_data thread_data_array[NUM_THREADS];

double **a, **b, **c;
int matrixSize;
int operation_counter = 0;

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

// Thread operation is a function created to controll the process for each thread, in this function each thread has to operate form an initial row to a final row, in order to calculate a block from the matrix multiplication operation
void *thread_operation(void *threadarg) {
  // Local variables to control the loops
  int j,k,i;
  // Struct with the information from the current thread
  struct thread_data *my_data;
  long t_id;
  double sum;
  // Call the especific information from the thread
  my_data = (struct thread_data *) threadarg;
  t_id = my_data->thread_id;
  // Debug message for development enviroment - Local tests
  printf("Hola soy el Thread #%ld, multiplicando desde la fila #%d hasta la fila #%d!\n", t_id, initial_row, final_row);
  // Iterate from the initial row to the final row from the current thread
  for (i = 0; i < matrixSize; i++) {
    for (j = 0; j < matrixSize; j++) {
      sum = 0.0;
      for (k = 0; k < matrixSize; k++) {
        sum = sum + my_data->matrix_a[i][k] * my_data->matrix_b[i][k];
      }
      thread_data_array[t_id].matrix_c[i][j] = sum;
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
  int i, j, k;
  int nmats;
  char *fname = "matrices_large.dat"; //Change to matrices_large.dat for performance evaluation
  FILE *fh;
  int operate = 1;

  printf("Start\n");
  fh = fopen(fname, "r");
  //First line indicates how many pairs of matrices there are and the matrix size
  fscanf(fh, "%d %d\n", &nmats, &matrixSize);

  long t;
  void *status;
  // Create threads as documents given with the initial code
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;
  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  //Dynamically create matrices of the size needed
  a = allocateMatrix();
  b = allocateMatrix();
  c = allocateMatrix();

  printf("Loading %d pairs of square matrices of size %d from %s...\n", nmats, matrixSize, fname);
  clock_t start = clock();

  if ( (nmats % NUM_THREADS) == 0 ) {
      // Assign and step to divide the matrix. Each thread will receive the same amount of rows in order to complete the operations
    while(operate==1){
      for(t=0; t<NUM_THREADS; t++){
        // Load a and b matrices from file for each thread
        for(i=0;i<matrixSize;i++){
          for(j=0;j<matrixSize;j++){
            fscanf(fh, "%lf", &thread_data_array[t].matrix_a[i][j]);
          }
        }
        for(i=0;i<matrixSize;i++){
          for(j=0;j<matrixSize;j++){
            fscanf(fh, "%lf", &thread_data_array[t].matrix_b[i][j]);
          }
        }
        printf("Multiplying two matrices...\n");
        // mm();
        // printResult();
        thread_data_array[t].thread_id = t;
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
      if (operation_counter == 50) {
        operate = 0;
      }
    }
    // Case 3


    } else {
      // In this case the amount of rows for each thread is not the same, we save the NUM_THREADS-1 thread to process the rest of the files and finis the operations
      step = matrixSize/NUM_THREADS;
      printf("***************\n");
      printf("%d\n", step);
      printf("***************\n");
      for(t=0; t<NUM_THREADS-1; t++){
        thread_data_array[t].thread_id = t;
        thread_data_array[t].initial_row = t * step;
        thread_data_array[t].final_row = (t+1)*step;
        rc = pthread_create(&threads[t], &attr, thread_operation,(void *) &thread_data_array[t]);
        if (rc){
          printf("ERROR; return code from pthread_create() is %d\n", rc);
          exit(-1);
        }
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