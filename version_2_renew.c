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
struct operation_data{
   int operation_id;
   double **matrix_a;
   double **matrix_b;
   double **matrix_c;
};

// Array to management all the operations with specific information for each of them matrix a and b
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

void printResult(int t_id){
  int i, j;
  for(i=0;i<matrixSize;i++){
    for(j=0;j<matrixSize;j++){
      printf("%lf ", results[t_id].matrix_a[i][j]);
    }
    printf("\n");
  }
  for(i=0;i<matrixSize;i++){
    for(j=0;j<matrixSize;j++){
      printf("%lf ", results[t_id].matrix_b[i][j]);
    }
    printf("\n");
  }
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

int main(void) {
  int i, j, k;
  int nmats;
  char *fname = "matrices_large.dat"; //Change to matrices_large.dat for performance evaluation
  FILE *fh;

  printf("Start\n");
  fh = fopen(fname, "r");
  //First line indicates how many pairs of matrices there are and the matrix size
  fscanf(fh, "%d %d\n", &nmats, &matrixSize);
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
    printResult(k);
  }
  exit(0);

  // printf("Multiplying two matrices...\n");
  // mm();
  // printResult();

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

