#ifndef KD_TREE_H
#define KD_TREE_H

#include "rec.h"
#include <set>
#include <vector>

typedef struct {
  int axis;
  Point point;
} Node;

class KDTree {
public:
  int axis;
  Point root;
  KDTree *left;
  KDTree *right;
  std::set<Point> points;
  KDTree(int ax, std::vector<Point> pts) {
    axis = ax;
    root = {-1, -1};
    left = nullptr;
    right = nullptr;
  }
};

KDTree *kd_init(std::vector<Point>, int);
bool sort_by_x(Point, Point);
bool sort_by_y(Point, Point);
KDTree *find_min(KDTree *, int);
KDTree *delete_node(KDTree *, Point);
void range_search(Rec &, KDTree *, Rec &, std::vector<Point> &);
#endif
