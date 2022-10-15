#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "knn.h"
#include "queue.h"
#include "utils.h"
#include "vp_tree.h"

int num_points;
int dim;

double **points;
double *distances;

double tau = RAND_MAX;
struct PQueue *queue;

void build_vpt(VPTree *node, int start, int end) {
  node->idx = end;
  node->inner = node->outer = NULL;
  node->md = 0;

  if (start == end) return;

  for (int i = start; i < end; i++)
    distances[i] = get_euclidean_distance(points[i], points[node->idx], dim);

  double md = get_median_distance(start, end);
  node->md = md;

  node->outer = malloc(sizeof(VPTree));
  build_vpt(node->outer, (start + end) / 2, end - 1);

  if ((start + end) / 2 - 1 < start) return;

  node->inner = malloc(sizeof(VPTree));
  build_vpt(node->inner, start, (start + end) / 2 - 1);
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

  // VPTree build
  VPTree *root = (VPTree *)malloc(sizeof(VPTree));
  start_t = clock();
  build_vpt(root, 0, num_points - 1);

  // Knn search
  knn(root, query, num_neighbours);

  printf("Ellapsed time: %lf\n", (double)(clock() - start_t) / CLOCKS_PER_SEC);

  //  print results
  //   while (q_size(&queue)) {
  //     printf("%d\n", peek(&queue)->id);
  //     dequeue(&queue);
  //   }

  return EXIT_SUCCESS;
}