#include "kd-tree.h"
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;
/*
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
*/

int tree_height(KDTree *root) {
  if (!root)
    return 0;
  int left_height = tree_height(root->left);
  int right_height = tree_height(root->right);
  return (left_height >= right_height) ? left_height + 1 : right_height + 1;
}

string print_level(KDTree *root, int level) {
  if (!root)
    return "";
  if (level == 0) {
    string axis = root->axis == 0 ? "x" : "y";
    return axis + ":(" + to_string(root->root.x) + "," +
           to_string(root->root.y) + ")";
  }
  return print_level(root->left, level - 1) +
         print_level(root->right, level - 1);
}

string test_kd_init_base_case() {
  vector<Point> pts = {{0, 0}, {1, 5}, {2, 2}, {7, 8}, {9, 9}};
  // x-sorted: {0,0},{1,5},{2,2},{7,8},{9,9}
  // y-sorted: {0,0},{2,2},{1,5},{7,8},{9,9}
  sort(pts.begin(), pts.end(), sort_by_x);
  KDTree *kd = kd_init(pts, 0);
  string correct_ans = "x:(2,2)y:(0,0)y:(7,8)x:(1,5)x:(9,9)";
  int height = tree_height(kd);
  string level_order = "";
  for (int i = 0; i < height; i++) {
    level_order += print_level(kd, i);
  }
  if (level_order == correct_ans)
    return "kd_init_base_case passed!";
  return "FAIL: kd_init_base_case returned " + level_order + " instead of " +
         correct_ans;
  /*
  if (kd->root.x != 2 && kd->root.y != 2)
    return {false, "kd_init_base_case 1 FAILED: intializer constructed wrong "
                   "root and used " +
                       to_string(kd->root.x) + " " + to_string(kd->root.y) +
                       " rather than 2,2"};

  if (kd->left->root.x != 2 && kd->right->root.y != 2)
    return {false, "kd_init_base_case 1 FAILED: intializer constructed wrong "
                   "left child and used " +
                       to_string(kd->left->root.x) + "," +
                       to_string(kd->left->root.y) + " rather than 2,2"};

  if (kd->right->root.x != 7 && kd->right->root.y != 8)
    return {false, "kd_init_base_case 1 FAILED: intializer constructed wrong "
                   "right child and used " +
                       to_string(kd->right->root.x) + "," +
                       to_string(kd->right->root.y) + " rather than 7,8"};
  */
}

string test_kd_init_base_case_2() {
  vector<Point> pts = {{1, 1}, {2, 2}, {3, 3}, {4, 4}};
  KDTree *kd = kd_init(pts, 0);
  string level_order = "";
  int height = tree_height(kd);
  for (int i = 0; i < height; i++) {
    level_order += print_level(kd, i);
  }

  string correct_ans = "x:(2,2)y:(1,1)y:(3,3)x:(4,4)";
  if (level_order == correct_ans)
    return "kd_init_base_case_2 passed!";
  return "FAIL: kd_init_base_case_2 returned " + level_order + " instead of " +
         correct_ans;
}

string test_find_min_base_case() {

  vector<Point> pts = {{1, 4}, {1, 5}, {3, 2}, {3, 6}, {7, 7}};
  KDTree *kd = kd_init(pts, 0);
  string level_order = "";
  int height = tree_height(kd);
  for (int i = 0; i < height; i++) {
    level_order += print_level(kd, i);
  }
  string correct_level_order = "x:(3,2)y:(1,4)y:(3,6)x:(1,5)x:(7,7)";
  if (level_order != correct_level_order)
    return "FAIL: find_min_base_case constructed " + level_order +
           " instead of " + correct_level_order;
  KDTree *root_min_x = find_min(kd, 0, 0);
  KDTree *root_min_y = find_min(kd, 1, 0);
  if (root_min_x->root.x != 1)
    return "FAIL: find_min return x min of " + to_string(root_min_x->root.x) +
           " instead of 1";
  if (root_min_y->root.y != 2)
    return "FAIL: find_min return y min of " + to_string(root_min_y->root.y) +
           " instead of 2";

  return "find_min_base_case passed!";
}

string test_delete_node_1() {
  vector<Point> pts = {{1, 1}, {4, 4}, {6, 6}};
  KDTree *kd = kd_init(pts, 0);
  string level_order = "";
  int height = tree_height(kd);
  for (int i = 0; i < height; i++) {
    level_order += print_level(kd, i);
  }
  string correct_level_order = "x:(4,4)y:(1,1)y:(6,6)";
  if (level_order != correct_level_order)
    return "FAIL: find_min_base_case constructed " + level_order +
           " instead of " + correct_level_order;

  kd = delete_node(kd, {4, 4});
  level_order = "";
  height = tree_height(kd);
  for (int i = 0; i < height; i++) {
    level_order += print_level(kd, i);
  }
  correct_level_order = "x:(6,6)y:(1,1)";
  if (level_order != correct_level_order)
    return "FAIL: find_min_base_case constructed " + level_order +
           " instead of " + correct_level_order;

  kd = delete_node(kd, {1, 1});
  level_order = "";
  height = tree_height(kd);
  for (int i = 0; i < height; i++) {
    level_order += print_level(kd, i);
  }
  correct_level_order = "x:(6,6)";
  if (level_order != correct_level_order)
    return "FAIL: find_min_base_case constructed " + level_order +
           " instead of " + correct_level_order;
  kd = delete_node(kd, {6, 6});
  level_order = "";
  height = tree_height(kd);
  for (int i = 0; i < height; i++) {
    level_order += print_level(kd, i);
  }
  correct_level_order = "";
  if (level_order != correct_level_order)
    return "FAIL: find_min_base_case constructed " + level_order +
           " instead of " + correct_level_order;

  return "delete_node_1 passed!";
}

int main() {
  string test1 = test_kd_init_base_case();
  cout << test1 << endl;

  string test2 = test_kd_init_base_case_2();
  cout << test2 << endl;

  string test3 = test_find_min_base_case();
  cout << test3 << endl;

  string test4 = test_delete_node_1();
  cout << test4 << endl;
  return 0;
}
