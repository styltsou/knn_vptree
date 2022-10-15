#ifndef STRUCTURES_H
#define STRUCTURES_H

typedef struct VPTree {
  int idx;
  double md;
  struct VPTree *inner;
  struct VPTree *outer;
} VPTree;

#endif