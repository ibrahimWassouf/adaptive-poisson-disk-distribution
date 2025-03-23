#include "kd-tree.h"
#include "rec.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <vector>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

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

int main() {

  // create rng
  random_device r;
  mt19937 e1(r());
  uniform_int_distribution<int> uniform_dist(1, 128);

  vector<Point> samples;
  const int N = 2000;
  // create random samples
  for (int i = 0; i < N; i++) {
    Point p = {uniform_dist(e1), uniform_dist(e1)};
    samples.push_back(p);
  }

  /*
  for (auto p : samples) {
    cout << "(" << p.first << "," << p.second << ") ";
  }
  */

  KDTree *kd = kd_init(samples, 0);

  const int AREA = 128 * 128;
  const int R_MAX = floor(sqrt(AREA / (2 * sqrt(3 * N))));

  // kd cell of entire ouput image
  Rec cell({0, 0}, {128, 128});

  set<pair<int, Point>, greater<>> heap;
  map<Point, set<pair<int, Point>>::iterator> dict;
  map<Point, vector<Point>> range_dict;

  // calculate weight of each sample
  for (auto p : samples) {
    vector<Point> within_range;
    Rec range({p.first - R_MAX, p.second - R_MAX},
              {p.first + R_MAX, p.second + R_MAX});

    range_search(range, kd, cell, within_range);
    range_dict[p] = within_range;
    int sum_w = 0;
    for (auto s : within_range) {
      int dist = (s.first - p.first) * (s.first - p.first) +
                 (s.second - p.second) * (s.second - p.second);
      dist = sqrt(dist);
      int w = 1 - (dist / (2 * R_MAX));
      sum_w += w;
    }
    /*
    cout << "Total weight: " << sum_w << endl;
    cout << "sample point: " << p.first << "," << p.second << endl;
    for (auto x : within_range) {
      cout << "(" << x.first << "," << x.second << ") ";
    }
    cout << endl;
    */

    auto insert_res = heap.insert({sum_w, p});
    // saves iterator position of sample weight in heap to facilitate a fast
    // update later
    dict[p] = insert_res.first;
  }

  // print the heap after weighing samples
  /*
  for (auto it : heap) {
    cout << it.first << " " << "(" << it.second.first << "," << it.second.second
         << ")" << endl;
  }
  */
  while (heap.size() > 1000) {

    // get the point with highest weight and remove it from the samples
    // and subtract it's weight from neighboring samples.
    auto p = heap.begin();
    vector<Point> within_range;

    Rec range({p->second.first - R_MAX, p->second.second - R_MAX},
              {p->second.first + R_MAX, p->second.second + R_MAX});

    range_search(range, kd, cell, within_range);
    for (auto x : within_range) {
      // grab iterator position of neighboring sample in the heap
      auto old_pos = dict[x];
      // update weight of neighboring sample in heap
      int dist = (old_pos->second.first - p->second.first) *
                     (old_pos->second.first - p->second.first) +
                 (old_pos->second.second - p->second.second) *
                     (old_pos->second.second - p->second.second);

      dist = sqrt(dist);
      int new_weight = old_pos->first - dist;

      heap.erase(old_pos);
      // return value is iterator,bool pair
      auto new_pos = heap.insert({new_weight, x});
      dict[x] = new_pos.first;
    }

    int height = tree_height(kd);
    string lo = "";
    for (int i = 0; i < height; i++) {
      lo += print_level(kd, i);
    }
    cout << heap.begin()->second.first << "," << heap.begin()->second.second
         << endl;
    cout << lo << endl;
    delete_node(kd, heap.begin()->second);
    heap.erase(heap.begin());
  }

  for (auto it : heap) {
    cout << it.first << " " << "(" << it.second.first << "," << it.second.second
         << ")" << endl;
  }
  return 0;
}
