#include <stdlib.h>

#include "queue.h"
#include "utils.h"
#include "vp_tree.h"

extern int dim;
extern double **points;
extern double tau;
extern struct PQueue *queue;

void knn(VPTree *node, double *query, int k) {
  if (node == NULL) return;

  double dist = get_euclidean_distance(query, points[node->idx], dim);

  if (dist < tau) {
    enqueue(&queue, node->idx, dist);
    if (q_size(&queue) > k) dequeue(&queue);
    if (q_size(&queue) == k) tau = peek(&queue)->priority;
  }

  if (dist < node->md) {
    if (dist - tau <= node->md) knn(node->inner, query, k);
    if (dist + tau >= node->md) knn(node->outer, query, k);
  } else {
    if (dist + tau >= node->md) knn(node->outer, query, k);
    if (dist - tau <= node->md) knn(node->inner, query, k);
  }
}