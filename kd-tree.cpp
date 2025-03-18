#include "kd-tree.h"
#include <algorithm>
#include <vector>
bool sort_by_x(Point p1, Point p2) {
  if (p1.x != p2.x)
    return p1.x < p2.x;
  return p1.y < p2.y;
}

bool sort_by_y(Point p1, Point p2) {
  if (p1.y != p2.y)
    return p1.y < p2.y;
  return p1.x < p2.x;
}

// Assumes pts parameter is a sorted array of input points
// axis value of 0 means the x-axis, 1 means y-axis
KDTree *kd_init(vector<Point> pts, int axis) {
  KDTree *kd = new KDTree(axis);
  if (axis == 0)
    sort(pts.begin(), pts.end(), sort_by_x);
  else
    sort(pts.begin(), pts.end(), sort_by_y);

  if (pts.size() == 0)
    return nullptr;
  if (pts.size() == 1) {
    kd->root = pts[0];
    return kd;
  } else if (pts.size() == 2) {
    kd->root = pts[0];
    kd->right = kd_init({pts[1]}, (axis + 1) % 2);
    return kd;
  } else {

    int median =
        pts.size() % 2 == 0 ? floor(pts.size() / 2) - 1 : floor(pts.size() / 2);
    kd->root = pts[median];
    vector<Point> left_pts(pts.begin(), pts.begin() + median);
    vector<Point> right_pts(pts.begin() + median + 1, pts.end());
    kd->left = kd_init(left_pts, (axis + 1) % 2);
    kd->right = kd_init(right_pts, (axis + 1) % 2);
    return kd;
  }
}

KDTree *min_kd(vector<KDTree *> &v, int dim) {
  // remove nullptr's
  for (int i = v.size() - 1; i >= 0; i--) {
    if (v[i] == nullptr) {
      v.erase(v.begin() + i);
    }
  }
  // sort by either x or y value
  if (dim == 0) {
    auto cmp = [](auto k1, auto k2) { return k1->root.x < k2->root.x; };

    sort(v.begin(), v.end(), cmp);
  } else {
    auto cmp = [](auto k1, auto k2) { return k1->root.y < k2->root.y; };
    sort(v.begin(), v.end(), cmp);
  }

  return v[0];
}

KDTree *find_min(KDTree *root, int target_dim, int dim) {
  if (!root)
    return nullptr;

  int next_dim = (dim + 1) % 2;

  // recurse on left child only
  if (target_dim == dim) {
    return root->left == nullptr ? root
                                 : find_min(root->left, target_dim, next_dim);
  }

  // recurse on both children
  vector<KDTree *> v = {root, find_min(root->left, target_dim, next_dim),
                        find_min(root->right, target_dim, next_dim)};
  return min_kd(v, target_dim);
}
