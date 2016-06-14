//Matrix multiplication


// Matrix multiplication - Versión 1
// Call pthread.h library
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Define number of threads
#define NUM_THREADS     2

// Thread Data structure to setup/pass multiple arguments via this structure. Each thread receives a unique instance of the structure.
struct thread_data{
   int thread_id;
   double **matrix_a;
   double **matrix_b;
   double **matrix_c;
};

// Array to management all the threads with specific information for each of them
struct thread_data thread_data_array[NUM_THREADS];

// Add mutex
pthread_mutex_t mutexcounter;

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
void *thread_operation(void *threadid) {
  // Local variables to control the loops
  int j,k,i;
  // Struct with the information from the current thread
  long t_id;
  double sum;
  // Call the especific information from the thread
  t_id = (long)threadid;
  // Debug message for development enviroment - Local tests
  printf("Hola soy el Thread #%ld,", t_id);
  // Iterate from the initial row to the final row from the current thread
  for (i = 0; i < matrixSize; i++) {
    for (j = 0; j < matrixSize; j++) {
      sum = 0.0;
      for (k = 0; k < matrixSize; k++) {
        sum = sum + thread_data_array[t_id].matrix_a[i][k] * thread_data_array[t_id].matrix_b[k][j];
      }
      thread_data_array[t_id].matrix_c[i][j] = sum;
    }
  }
  pthread_mutex_lock (&mutexcounter);
  operation_counter++;
  pthread_mutex_unlock (&mutexcounter);
  pthread_exit(NULL);
}

// void mm(void) {
//   int i,j,k;
//   double sum;

//   for (i = 0; i < matrixSize; i++) {
//     for (j = 0; j < matrixSize; j++) {
//       sum = 0.0;
//       for (k = 0; k < matrixSize; k++) {
//         sum = sum + a[i][k] * b[k][j];
//       }
//       c[i][j] = sum;
//     }
//   }
// }

void printResult(int t_id){
  int i, j;
  for(i=0;i<matrixSize;i++){
    for(j=0;j<matrixSize;j++){
      printf("%lf ", thread_data_array[t_id].matrix_b[i][j]);
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
  int rc;
  // Create threads as documents given with the initial code
  pthread_t threads[NUM_THREADS];
  // Define thread attributes by default
  pthread_attr_t attr;
  // Initialize mutex
  // pthread_mutex_init(&mutexcounter, NULL);
  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);

  for(k=0;k<NUM_THREADS;k++){
    thread_data_array[k].matrix_a=allocateMatrix();
    thread_data_array[k].matrix_b=allocateMatrix();
    thread_data_array[k].matrix_c=allocateMatrix();
  }

  printf("Loading %d pairs of square matrices of size %d from %s...\n", nmats, matrixSize, fname);
  clock_t start = clock();

  if ( (nmats % NUM_THREADS) == 0 ) {
      // Assign and step to divide the matrix. Each thread will receive the same amount of rows in order to complete the operations
    while(operate==1){
      pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
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
        printf("Hola antes de morir thread #%ld\n",t);
        rc = pthread_create(&threads[t], &attr, thread_operation,(void *) t);
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
  }
  clock_t end = clock();
  printf("************* \n");
  double time_elapsed_in_seconds = (end - start)/(double)CLOCKS_PER_SEC;
  printf("The time for the operation was %f \n", time_elapsed_in_seconds);
  printf("************* \n");
  fclose(fh);

  // // Free memory
  for(k=0;k<nmats;k++){
    free(*thread_data_array[t].matrix_a);
    free(thread_data_array[t].matrix_a);
    free(*thread_data_array[t].matrix_b);
    free(thread_data_array[t].matrix_b);
    free(*thread_data_array[t].matrix_c);
    free(thread_data_array[t].matrix_c);
  }
  printf("Done.\n");
  pthread_mutex_destroy(&mutexcounter);pthread_exit(NULL);
  // pthread_exit(NULL);
  return 0;
}