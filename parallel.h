#ifndef PARALLEL_H
#define PARALLEL_H
#include "vp_tree.h"

typedef struct {
  int start;
  int end;
  int vp_id;
} DistTargs;

typedef struct {
  VPTree *node;
  int start;
  int end;
} TreeTArgs;

void update_active_threads(int amount);

void *calc_distances(void *args);
void calc_distances_parallel(int n, int num_t, int vp_id);

void *build_vpt_parallel(void *args);
void build_vpt(VPTree *node, int start, int end);

#endif