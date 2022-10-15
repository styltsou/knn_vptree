#ifndef UTILS_H
#define UTILS_H
#include "vp_tree.h"

void swap_double(double *a, double *b);
void swap_double_array(double **a, double **b);
int partition(int l, int r);
double quickselect(int l, int r, int k);

double get_euclidean_distance(double *p1, double *p2, int dim);
double get_median_distance(int start, int end);

#endif