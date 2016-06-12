#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void *task(int id) {
  printf("Task %d started\n", id);
  int i;
  double result = 0.0;
  for (i = 0; i < 1000000; i++) {
    result = result + sin(i) * tan(i);
  }
  printf("Task %d completed with result %e\n", id, result);
}

void *threaded_task(void *t) {
  long id = (long) t;
  printf("Thread %ld started\n", id);
  task(id);
  printf("Thread %ld done\n", id);
  pthread_exit(0);
}

void *parallel(int num_tasks)
{
  int num_threads = num_tasks;
  pthread_t thread[num_threads];
  int rc;
  long t;
  for (t = 0; t < num_threads; t++) {
    printf("Creating thread %ld\n", t);
    rc = pthread_create(&thread[t], NULL, threaded_task, (void *)t);
    if (rc) {
      printf("ERROR: return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
  }
}

int main(void) {
  int num_tasks = 2;
  parallel(num_tasks);

  printf("Main completed\n");
  pthread_exit(NULL);
}