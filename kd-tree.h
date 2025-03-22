#ifndef KD_TREE_H
#define KD_TREE_H

#include <vector>

struct Point {
  int x;
  int y;

  bool operator==(const Point &p) { return x == p.x && y == p.y; };
  bool operator!=(const Point &p) { return x != p.x || y != p.y; };
};

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
  KDTree(int ax) {
    axis = ax;
    root = {-1, -1};
    left = nullptr;
    right = nullptr;
  }
};

KDTree *kd_init(std::vector<Point>, int);
bool sort_by_x(Point, Point);
bool sort_by_y(Point, Point);
KDTree *find_min(KDTree *, int, int);
KDTree *delete_node(KDTree *, Point);
#endif
