//Matrix multiplication

// Matrix multiplication - Versión 1
// Call pthread.h library
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define number of threads
#define NUM_THREADS     8

// Thread Data structure to setup/pass multiple arguments via this structure. Each thread receives a unique instance of the structure with matrix a and b
struct operation_data{
   int operation_id;
   double **matrix_a;
   double **matrix_b;
   double **matrix_c;
};

// Array to management all the operations with specific information for each of them matrix a and b and the final result
struct operation_data results[25];

// Thread Data structure to setup/pass multiple arguments via this structure. Each thread receives a unique instance of the structure.
struct thread_data{
   int thread_id;
   int initial_operation;
   int final_operation;
};

// Array to management all the threads with specific information for each of them
struct thread_data thread_data_array[NUM_THREADS];

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

// Print result function in order to show all the results
void printResult(void){
  int i, j, k;
  for(k=0;k<25;k++){
    for(i=0;i<matrixSize;i++){
      for(j=0;j<matrixSize;j++){
        printf("%lf ", results[k].matrix_c[i][j]);
      }
      printf("\n");
    }
  }
}

// Thread operation is a function created to controll the process for each thread, in this function each thread has to operate form an initial operation to a final operation, in order to calculate a group of matrices multiplications
void *thread_operation(void *threadarg) {
  // Local variables to control the loops
  int j,k,i,initial_op,final_op,z;
  // Struct with the information from the current thread
  struct thread_data *my_data;
  long t_id;
  double sum;
  // Call the especific information from the thread
  my_data = (struct thread_data *) threadarg;
  t_id = my_data->thread_id;
  initial_op = my_data->initial_operation;
  final_op = my_data->final_operation;
  // Debug message for development enviroment - Local tests
  printf("Hola soy el Thread #%ld, multiplicando desde la operación #%d hasta la operación #%d!\n", t_id, initial_op+1, final_op);
  // Multiplicate from the initial row to the final row from the current operation result
  for (z = initial_op; z < final_op; z++) {
    for (i = 0; i < matrixSize; i++) {
      for (j = 0; j < matrixSize; j++) {
        sum = 0.0;
        for (k = 0; k < matrixSize; k++) {
          sum = sum + results[z].matrix_a[i][k] * results[z].matrix_b[k][j];
        }
        results[z].matrix_c[i][j] = sum;
      }
    }
  }
  pthread_exit(NULL);
}

// To use when the user wants a single thread
void mm(void) {
  int i,j,k,z;
  double sum;
  for (z = 0; z < 25; z++) {
    for (i = 0; i < matrixSize; i++) {
      for (j = 0; j < matrixSize; j++) {
        sum = 0.0;
        for (k = 0; k < matrixSize; k++) {
          sum = sum + results[z].matrix_a[i][k] * results[z].matrix_b[k][j];
        }
        results[z].matrix_c[i][j] = sum;
      }
    }
  }
}

int main(void) {
  int i, j, k, nmats, rc, ops_by_thread;
  char *fname = "matrices_large.dat"; //Change to matrices_large.dat for performance evaluation
  FILE *fh;

  printf("Start\n");
  fh = fopen(fname, "r");
  //First line indicates how many pairs of matrices there are and the matrix size
  fscanf(fh, "%d %d\n", &nmats, &matrixSize);
  // We were making a big mistake the number of operations was 25 and not 50. 50 is the number of matrices to load
  nmats = 25;

  //Dynamically create matrices of the size needed
  for(k=0;k<nmats;k++){
    results[k].matrix_a=allocateMatrix();
    results[k].matrix_b=allocateMatrix();
    results[k].matrix_c=allocateMatrix();
  }
  printf("Loading %d pairs of square matrices of size %d from %s...\n", nmats, matrixSize, fname);
  clock_t start = clock();
  for(k=0;k<nmats;k++){
    for(i=0;i<matrixSize;i++){
      for(j=0;j<matrixSize;j++){
        fscanf(fh, "%lf", &results[k].matrix_a[i][j]);
      }
    }
    for(i=0;i<matrixSize;i++){
      for(j=0;j<matrixSize;j++){
        fscanf(fh, "%lf", &results[k].matrix_b[i][j]);
      }
    }
  }

  long t;
  void *status;
  // Create threads as documents given with the initial code
  pthread_t threads[NUM_THREADS];
  pthread_attr_t attr;
  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  // After we have the threads we going to evaluate three special cases
  // 1. When the number of threads is equal to one
  // 2. When the number of multiplications is exactly divisible by the number of threads
  // 3. When the number of multiplications is not exactly divisible by the number of threads and we need to do special considerations
  // Case 1
  if( NUM_THREADS == 1 ) {
    // Run the normal matrix multiplication function
    mm();
    printResult();
  // Case 2
  } else if ( (nmats % NUM_THREADS) == 0 ) {
    // Assign and step to divide the operations. Each thread will receive the same amount of operations in order to complete the multiplications
    ops_by_thread = nmats/NUM_THREADS;
    for(t=0; t<NUM_THREADS; t++){
      thread_data_array[t].thread_id = t;
      thread_data_array[t].initial_operation = t * ops_by_thread;
      thread_data_array[t].final_operation = (t+1)*ops_by_thread;
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
    printResult();
  } else if ( (nmats % NUM_THREADS) == 1 ) {
    // Assign a balance to divide the operations. Each thread will receive almost the same amount of operations in order to complete the multiplications
    printf("ENTREEEEEE\n");
    ops_by_thread = nmats/NUM_THREADS;
    for(t=0; t<NUM_THREADS; t++){
      thread_data_array[t].thread_id = t;
      thread_data_array[t].initial_operation = t * ops_by_thread;
      // Special case to balance the operations
      if ( t == (NUM_THREADS-1)) {
        thread_data_array[t].final_operation = ((t+1)*ops_by_thread)+1;
      } else {
        thread_data_array[t].final_operation = (t+1)*ops_by_thread;
      }
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
    printResult();
  } else if ( (nmats % NUM_THREADS) == 4 ) {
    // Assign and step to divide the matrix. Each thread will receive the same amount of rows in order to complete the operations
    ops_by_thread = nmats/NUM_THREADS;
    for(t=0; t<NUM_THREADS; t++){
      thread_data_array[t].thread_id = t;
      thread_data_array[t].initial_operation = t * ops_by_thread;
      // Special case to balance the operations
      if (t == (NUM_THREADS-1)) {
        thread_data_array[t].final_operation = ((t+1)*ops_by_thread)+4;
      } else {
        thread_data_array[t].final_operation = (t+1)*ops_by_thread;
      }
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
    printResult();
  }
  clock_t end = clock();
  printf("************* \n");
  double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
  printf("The time for the operation was %f \n", time_elapsed_in_seconds);
  printf("************* \n");
  fclose(fh);

  // Free memory
  for(k=0;k<nmats;k++){
    free(*results[k].matrix_a);
    free(results[k].matrix_a);
    free(*results[k].matrix_b);
    free(results[k].matrix_b);
    free(*results[k].matrix_c);
    free(results[k].matrix_c);
  }
  printf("Done.\n");
  return 0;
}