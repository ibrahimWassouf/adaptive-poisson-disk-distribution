#include "kd-tree.h"
#include "rec.h"
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
    return axis + ":(" + to_string(root->root.first) + "," +
           to_string(root->root.second) + ")";
  }
  return print_level(root->left, level - 1) +
         print_level(root->right, level - 1);
}

string vec_to_string(vector<Point> &pts) {
  string res = "";
  for (auto x : pts) {
    res += to_string(x.first) + "," + to_string(x.second) + " ";
  }
  return res;
}

bool check_pts_equality(vector<Point> &p1, vector<Point> &p2) {
  if (p1.size() != p2.size())
    return false;

  sort(p1.begin(), p1.end());
  sort(p2.begin(), p2.end());
  auto it = p1.begin();
  auto it2 = p2.begin();
  for (; it != p1.end(); it++, it2++) {
    if (*it != *it2)
      return false;
  }
  return true;
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
  if (root_min_x->root.first != 1)
    return "FAIL: find_min return x min of " +
           to_string(root_min_x->root.first) + " instead of 1";
  if (root_min_y->root.second != 2)
    return "FAIL: find_min return y min of " +
           to_string(root_min_y->root.second) + " instead of 2";

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

// tests when point is/not contained
string test_contains_point_1() {
  // Rec *r = new Rec({1, 1}, {8, 8});
  Rec r({1, 1}, {8, 8});
  Point p = {3, 3};
  Point p2 = {9, 7};
  bool t1 = contains_point(r, p);
  bool t2 = contains_point(r, p2);
  if (!t1)
    return "FAIL point 3,3 should have been contained";
  if (t2)
    return "FAIL point 9,7 should NOT have been contained";
  return "contains_point test passed!";
}

// test that cell is fully contained
string test_contains_cell_1() {
  Rec r({1, 1}, {8, 8});
  // cell is contained
  Rec c({4, 4}, {7, 7});
  // cell is outside left bound
  Rec c2({0, 2}, {4, 4});
  // cell is outside bottom bound
  Rec c3({2, 0}, {4, 4});
  // cell is outside right bound
  Rec c4({7, 4}, {9, 7});
  // cell is outside top bound
  Rec c5({6, 6}, {7, 9});

  if (contains_cell(r, c2))
    return "FAIL (1,1)(8,8) returned that it contains (0,2)(4,4)";

  if (contains_cell(r, c3))
    return "FAIL (1,1)(8,8) returned that it contains (2,0)(4,4)";

  if (contains_cell(r, c4))
    return "FAIL (1,1)(8,8) returned that it contains (7,4)(9,7)";

  if (contains_cell(r, c5))
    return "FAIL (1,1)(8,8) returned that it contains (6,6)(7,9)";

  if (contains_cell(r, c))
    return "contains_cell_1 test passed!";
  else
    return "FAIL (1,1)(8,8) returned that it did NOT contain (4,4)(7,7)";
}

string test_is_disjoint() {
  Rec r({1, 1}, {8, 8});
  // cell is contained
  Rec c({4, 4}, {7, 7});
  // cell is outside left bound
  Rec c2({0, 2}, {4, 4});
  // cell is outside bottom bound
  Rec c3({2, 0}, {4, 4});
  // cell is outside right bound
  Rec c4({7, 4}, {9, 7});
  // cell is outside top bound
  Rec c5({6, 6}, {7, 9});
  // cell is disjoint to the left
  Rec c6({-5, 0}, {0, 5});
  // cell is disjoint to the right
  Rec c7({9, 1}, {10, 9});

  if (is_disjoint(r, c2))
    return "FAIL (1,1)(8,8) returned that it is disjoint from (0,2)(4,4)";

  if (is_disjoint(r, c3))
    return "FAIL (1,1)(8,8) returned that it is disjoint from (2,0)(4,4)";

  if (is_disjoint(r, c4))
    return "FAIL (1,1)(8,8) returned that it is disjoint from (7,4)(9,7)";

  if (is_disjoint(r, c5))
    return "FAIL (1,1)(8,8) returned that it is disjoint from (6,6)(7,9)";

  if (is_disjoint(r, c))
    return "FAIL (1,1)(8,8) return that it is disjoint from (4,4)(7,7)";
  if (is_disjoint(r, c6) && is_disjoint(r, c7))
    return "is_disjoint test passed!";
  else
    return "FAIL (1,1)(8,8) returned that it is NOT from (-5,0)(0,5) or "
           "(9,1)(10,9)";
}

string test_left_rec() {
  Rec r({0, 0}, {10, 10});
  Point median = {5, 5};
  Rec left_rec_x = left_rec(r, 0, median);
  Rec left_rec_y = left_rec(r, 1, median);
  if (left_rec_x.bottom_left != (Point){0, 0} ||
      left_rec_x.top_right != (Point){5, 10})
    return "FAIL left_rec split on x axis did not return rec with bottom_left "
           "(0,0) and top_right (5,10)";
  if (left_rec_y.bottom_left != (Point){0, 0} ||
      left_rec_y.top_right != (Point){10, 5})
    return "FAIL left_rec split on y axis did not return rec with bottom_left "
           "(0,0) and top_right (10,5)";

  return "left_rec test passed!";
}

string test_right_rec() {
  Rec r({0, 0}, {10, 10});
  Point median = {5, 5};
  Rec right_rec_x = right_rec(r, 0, median);
  Rec right_rec_y = right_rec(r, 1, median);
  if (right_rec_x.bottom_left != (Point){5, 0} ||
      right_rec_x.top_right != (Point){10, 10})
    return "FAIL right_rec split on x axis did not return rec with bottom_left "
           "(0,5) and top_right (10,10)";
  if (right_rec_y.bottom_left != (Point){0, 5} ||
      right_rec_y.top_right != (Point){10, 10})
    return "FAIL right_rec split on y axis did not return rec with bottom_left "
           "(0,5) and top_right (10,10)";

  return "right_rec test passed!";
}

string test_range_search_1() {
  vector<Point> pts = {{1, 1}, {2, 2}, {3, 3}, {3, 5},  {4, 4}, {5, 3},
                       {5, 5}, {5, 6}, {6, 5}, {6, 6},  {7, 3}, {7, 7},
                       {8, 8}, {9, 5}, {9, 9}, {10, 10}};
  KDTree *kd = kd_init(pts, 0);
  Rec range({4, 4}, {7, 7});
  Rec kd_cell({0, 0}, {10, 10});
  vector<Point> res = {};
  range_search(range, kd, kd_cell, res);
  vector<Point> expected = {{4, 4}, {5, 5}, {5, 6}, {6, 5}, {6, 6}, {7, 7}};
  sort(res.begin(), res.end());
  if (!check_pts_equality(res, expected))
    return "FAIL range_search over (4,4)(7,7) returned " + vec_to_string(res) +
           " rather than " + vec_to_string(expected);
  return "range_search_1 passed!";
}

string test_range_search_2() {
  vector<Point> pts = {{1, 1}, {2, 2}, {3, 3}, {3, 5},  {4, 4}, {5, 3},
                       {5, 5}, {5, 6}, {6, 5}, {6, 6},  {7, 3}, {7, 7},
                       {8, 8}, {9, 5}, {9, 9}, {10, 10}};
  KDTree *kd = kd_init(pts, 0);
  Rec range({8, 0}, {10, 2});
  Rec kd_cell({0, 0}, {10, 10});
  vector<Point> res = {};
  range_search(range, kd, kd_cell, res);
  vector<Point> expected = {};
  if (!check_pts_equality(res, expected))
    return "FAIL range_search over (8,0)(10,2) returned " + vec_to_string(res) +
           " rather than " + vec_to_string(expected);
  return "range_search_2 passed!";
}

string test_range_search_3() {
  vector<Point> pts = {{1, 1}, {2, 2}, {3, 3}, {3, 5},  {4, 4}, {5, 3},
                       {5, 5}, {5, 6}, {6, 5}, {6, 6},  {7, 3}, {7, 7},
                       {8, 8}, {9, 5}, {9, 9}, {10, 10}};
  KDTree *kd = kd_init(pts, 0);
  Rec range({7, 3}, {9, 5});
  Rec kd_cell({0, 0}, {10, 10});
  vector<Point> res = {};
  range_search(range, kd, kd_cell, res);
  vector<Point> expected = {{7, 3}, {9, 5}};
  if (!check_pts_equality(res, expected))
    return "FAIL range_search over (7,3)(9,5) returned " + vec_to_string(res) +
           " rather than " + vec_to_string(expected);
  return "range_search_3 passed!";
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

  string test5 = test_contains_point_1();
  cout << test5 << endl;

  string test6 = test_contains_cell_1();
  cout << test6 << endl;

  string test7 = test_is_disjoint();
  cout << test7 << endl;

  string test8 = test_left_rec();
  cout << test8 << endl;

  string test9 = test_right_rec();
  cout << test9 << endl;

  // range search test 1
  string test10 = test_range_search_1();
  cout << test10 << endl;

  // range search test 2
  string test11 = test_range_search_2();
  cout << test11 << endl;

  // range search test 3
  string test12 = test_range_search_3();
  cout << test12 << endl;
  return 0;
}
