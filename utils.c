#include "utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "vp_tree.h"

extern double **points;
extern double *distances;

void swap_double(double *a, double *b) {
  double temp = *a;
  *a = *b;
  *b = temp;
}

void swap_double_array(double **a, double **b) {
  double *temp = *a;
  *a = *b;
  *b = temp;
}

int partition(int l, int r) {
  double x = distances[r];
  int i = l;
  for (int j = l; j <= r - 1; j++) {
    if (distances[j] <= x) {
      swap_double(&distances[i], &distances[j]);
      swap_double_array(&points[i], &points[j]);
      i++;
    }
  }
  swap_double(&distances[i], &distances[r]);
  swap_double_array(&points[i], &points[r]);
  return i;
}

double quickselect(int l, int r, int k) {
  if (k > 0 && k <= r - l + 1) {
    int index = partition(l, r);

    if (index - l == k - 1) return distances[index];

    if (index - l > k - 1) return quickselect(l, index - 1, k);

    return quickselect(index + 1, r, k - index + l - 1);
  }
  return 0;
}

double get_euclidean_distance(double *p1, double *p2, int dim) {
  double distance = 0.0;
  for (int i = 0; i < dim; i++) distance += pow(p2[i] - p1[i], 2.0);
  return sqrt(distance);
}

double get_median_distance(int start, int end) {
  return (quickselect(start, end - 1, (end - start - 1) / 2 + 1) +
          quickselect(start, end - 1, (end - start) / 2 + 1)) /
         2;
}