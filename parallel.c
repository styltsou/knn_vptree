#include "parallel.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <time.h>

#include "knn.h"
#include "queue.h"
#include "utils.h"
#include "vp_tree.h"

int MAX_THREADS;
int MIN_WORK_PER_THREAD = 100;
int active_threads;

int num_points = 100;
int dim = 2;

double **points;
double *distances;

double tau = RAND_MAX;
struct PQueue *queue;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void update_active_threads(int amount) {
  pthread_mutex_lock(&mutex);
  active_threads += amount;
  pthread_mutex_unlock(&mutex);
}

void *calc_distances(void *args) {
  DistTargs *targs = (DistTargs *)args;

  for (int i = targs->start; i < targs->end; i++)
    distances[i] = get_euclidean_distance(points[i], points[targs->vp_id], dim);
}

void calc_distances_parallel(int n, int num_t, int vp_id) {
  pthread_t threads[num_t];
  DistTargs targs[num_t];

  for (int i = 0; i < num_t; i++) {
    targs[i].start = 0;

    for (int j = 0; j < i; j++)
      targs[i].start += (j < n % num_t) ? (n / num_t + 1) : (n / num_t);

    targs[i].end =
        targs[i].start + ((i < n % num_t) ? (n / num_t + 1) : (n / num_t));

    targs[i].vp_id = vp_id;

    pthread_create(&threads[i], NULL, &calc_distances, &targs[i]);
    pthread_join(threads[i], NULL);
  }
}

void *build_vpt_parallel(void *args) {
  TreeTArgs *targs = (TreeTArgs *)args;
  // Call make_vpt with the appropriate arguments
  build_vpt(targs->node, targs->start, targs->end);
  // Decrease number of threads by one
  update_active_threads(-1);
  pthread_exit(NULL);
  return (void *)0;
}

void build_vpt(VPTree *node, int start, int end) {
  node->idx = end;
  node->md = 0;
  node->inner = node->outer = NULL;

  if (start == end) return;

  // Find number of available threads
  int free_threads = MAX_THREADS - active_threads;
  //  Choosing sequential  or parallel depending on thread availability
  if (free_threads > 0 && (end - start + 1) / 2 > MIN_WORK_PER_THREAD) {
    update_active_threads(+free_threads);
    calc_distances_parallel(end - start, free_threads, node->idx);
    update_active_threads(-free_threads);
  } else {
    // Calcualate distances sequentially
    for (int i = start; i < end; i++)
      distances[i] = get_euclidean_distance(points[i], points[node->idx], dim);
  }

  // Find median distance of current node's set (exclude the vantage point)
  node->md = get_median_distance(start, end);

  // Find the bounds of inner and outer sub-trees
  int start_inner = start;
  int end_inner = (start + end) / 2 - 1;
  int start_outer = end_inner + 1;
  int end_outer = end - 1;

  // Decide if should build the inner vpt using a seperate thread
  if (free_threads && (end - start + 1) / 2 > MIN_WORK_PER_THREAD) {
    // Create args to pass to thread
    node->inner = malloc(sizeof(VPTree));
    pthread_t t_inner;
    TreeTArgs args_inner;
    args_inner.node = node->inner;
    args_inner.start = start_inner;
    args_inner.end = end_inner;

    // First increase the number of active threads by one
    update_active_threads(+1);
    pthread_create(&t_inner, NULL, &build_vpt_parallel, (void *)&args_inner);
    pthread_join(t_inner, NULL);

    node->outer = malloc(sizeof(VPTree));
    build_vpt(node->outer, start_outer, end_outer);

    update_active_threads(-1);
  } else {
    node->outer = malloc(sizeof(VPTree));
    build_vpt(node->outer, start_outer, end_outer);

    if ((start + end) / 2 - 1 < start) return;

    node->inner = malloc(sizeof(VPTree));
    build_vpt(node->inner, start_inner, end_inner);
  }
}

int main(int argc, char *argv[]) {
  srand(0);

  if (argc < 4) {
    printf("Missing arguments...\n");
  }

  num_points = atoi(argv[1]);
  dim = atoi(argv[2]);
  int num_neighbours = atoi(argv[3]);

  clock_t start_t;

  // Allocate space for points and generate a random dataset
  points = (double **)malloc(num_points * sizeof(double *));
  for (int i = 0; i < num_points; i++) {
    points[i] = (double *)malloc(dim * sizeof(double));
  }

  for (int i = 0; i < num_points; i++) {
    for (int j = 0; j < dim; j++) {
      points[i][j] = (double)rand() / RAND_MAX;
    }
  };

  // Allocate space to store computed distances
  distances = (double *)malloc(num_points * sizeof(double));

  // Generate a random query
  double *query = (double *)malloc(dim * sizeof(double));
  for (int i = 0; i < dim; i++) query[i] = (double)rand() / RAND_MAX;

  // Init queue to store nearest neighbours
  queue = newNode(-1, RAND_MAX);

  // get number of available threads
  MAX_THREADS = get_nprocs();

  // VPTree build
  VPTree *root = (VPTree *)malloc(sizeof(VPTree));
  start_t = clock();
  build_vpt(root, 0, num_points - 1);

  // Knn search
  knn(root, query, num_neighbours);

  printf("Ellapsed time: %lf\n", (double)(clock() - start_t) / CLOCKS_PER_SEC);
  //  print results
  // while (q_size(&queue)) {
  //   printf("%d\n", peek(&queue)->id);
  //   dequeue(&queue);
  // }

  return EXIT_SUCCESS;
}