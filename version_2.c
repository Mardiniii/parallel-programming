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






  pthread_t threads[NUM_THREADS];

  int rc;
  long t;
  for(t=0; t<NUM_THREADS; t++){
    printf("In main: creating thread %ld\n", t);
    rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
  /* Last thing that main() should do */
  pthread_exit(NULL);

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