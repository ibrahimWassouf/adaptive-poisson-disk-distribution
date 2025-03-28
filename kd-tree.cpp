#include "kd-tree.h"
#include "rec.h"
#include <algorithm>
#include <iostream>
#include <vector>
using namespace std;
bool sort_by_x(Point p1, Point p2) {
  if (p1.first != p2.first)
    return p1.first < p2.first;
  return p1.second < p2.second;
}

bool sort_by_y(Point p1, Point p2) {
  if (p1.second != p2.second)
    return p1.second < p2.second;
  return p1.first < p2.first;
}

// Assumes pts parameter is a sorted array of input points
// axis value of 0 means the x-axis, 1 means y-axis
KDTree *kd_init(vector<Point> pts, int axis) {
  KDTree *kd = new KDTree(axis, pts);
  if (axis == 0)
    sort(pts.begin(), pts.end(), sort_by_x);
  else
    sort(pts.begin(), pts.end(), sort_by_y);

  if (pts.size() == 0)
    return nullptr;
  if (pts.size() == 1) {
    kd->root = pts[0];
    kd->axis = axis;
    return kd;
  }
  if (pts.size() == 2) {
    kd->root = pts[0];
    kd->axis = axis;
    kd->points.insert(pts.begin(), pts.end());
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
    kd->points.insert(pts.begin(), pts.end());
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
    auto cmp = [](auto k1, auto k2) { return k1->root.first < k2->root.first; };

    sort(v.begin(), v.end(), cmp);
  } else {
    auto cmp = [](auto k1, auto k2) {
      return k1->root.second < k2->root.second;
    };
    sort(v.begin(), v.end(), cmp);
  }

  return v[0];
}

KDTree *find_min(KDTree *root, int target_dim) {
  if (!root)
    return nullptr;

  // recurse on left child only
  if (target_dim == root->axis) {
    return root->left == nullptr ? root : find_min(root->left, target_dim);
  }

  // recurse on both children
  vector<KDTree *> v = {root, find_min(root->left, target_dim),
                        find_min(root->right, target_dim)};
  return min_kd(v, target_dim);
}

KDTree *delete_node(KDTree *kd, Point p) {
  if (!kd)
    return nullptr;

  // cases where we found the point we want to delete
  if (p == kd->root) {
    if (kd->right) {
      KDTree *min_right = find_min(kd->right, kd->axis);
      kd->root = min_right->root;
      kd->right = delete_node(kd->right, min_right->root);
    } else if (kd->left) {
      KDTree *min_left = find_min(kd->left, kd->axis);
      kd->root = min_left->root;
      kd->right = delete_node(kd->left, min_left->root);
      kd->left = nullptr;
    } else {
      kd = nullptr;
      return nullptr;
    }
    // cases to search for point in left child
  } else if ((kd->axis == 0 && kd->root.first > p.first) ||
             (kd->axis == 1 && kd->root.second > p.second))
    kd->left = delete_node(kd->left, p);
  // cases to search for point in right child
  else if ((kd->axis == 0 && kd->root.first < p.first) ||
           (kd->axis == 1 && kd->root.second < p.second))
    kd->right = delete_node(kd->right, p);

  // fix set of points in subtree

  // kd->points.erase(find(kd->points.begin(), kd->points.end(), p));
  /*
  vector<Point> left_pts = kd->left ? kd->left->points : vector<Point>(0);
  vector<Point> right_pts = kd->right ? kd->right->points : vector<Point>(0);
  kd->points = {kd->root};
  kd->points.insert(kd->points.end(), left_pts.begin(), left_pts.end());
  kd->points.insert(kd->points.end(), right_pts.begin(), right_pts.end());
  */
  if (kd->points.find(p) != kd->points.end())
    kd->points.erase(kd->points.find(p));
  return kd;
}

void range_search(Rec &search_range, KDTree *node, Rec &kd_cell,
                  vector<Point> &res) {

  if (!node || is_disjoint(search_range, kd_cell))
    return;

  if (contains_cell(search_range, kd_cell)) {
    for (auto x : node->points)
      res.push_back(x);
    return;
  }

  if (contains_point(search_range, node->root)) {
    res.push_back(node->root);
  }

  Rec left = left_rec(kd_cell, node->axis, node->root);
  Rec right = right_rec(kd_cell, node->axis, node->root);
  range_search(search_range, node->left, left, res);
  range_search(search_range, node->right, right, res);
}
