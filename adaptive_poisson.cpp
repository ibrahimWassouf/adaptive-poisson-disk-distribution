#include "kd-tree.h"
#include "rec.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <random>
#include <set>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

  int x, y, n, ok;

  ok = stbi_info("gray_test.jpg", &x, &y, &n);
  if (!ok) {
    cout << "file format not supported\n";
    return 1;
  }

  unsigned char *gr_char = stbi_load("gray_test.jpg", &x, &y, &n, 0);
  cout << "read file\n";

  const double M = 18000;
  const double N = 6000;
  const double ALPHA = 8;
  const double GAMMA = 1.5;
  const double BETA = 0.95;

  const int DIM = max(x, y);
  const double AREA = DIM * DIM;
  const double R_MAX_2D = sqrt(AREA / (2.0 * sqrt(3.0) * N));
  const double R_MAX = 3 * R_MAX_2D;
  const double R_MIN = R_MAX / 4 * (1 - pow(N / M, GAMMA)) * BETA;

  vector<vector<double>> gradient(DIM, vector<double>(DIM, 0));
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      /*
      double sum = 0;
      for (int k = 0; k < n; k++) {
        sum += gr_char[i * x + j];
      }
      sum /= n;
      */
      gradient[i][j] = gr_char[i * x + j];
    }
  }
  cout << "What's in gr_char: " << gr_char[0] << endl;

  /*
  for (int i = 0; i < DIM; i++) {
    for (int j = 0; j < DIM; j++) {
      gradient[i][j] = min(2 * j, 255);
      // cout << img[i][j] << " ";
    }
    // cout << endl;
  }
  */

  // create rng
  random_device r;
  seed_seq seed{r(), r(), r(), r()};
  mt19937 e1;
  e1.seed(seed);
  uniform_int_distribution<int> uniform_dist(0, DIM - 1);
  // create random samples
  vector<Point> samples;
  set<Point> uniq;
  while (uniq.size() < M) {
    int x = uniform_dist(e1);
    int y = uniform_dist(e1);
    Point p = {x, y};
    /*
    Point p = {floor((double)rand() / RAND_MAX * DIM),
               floor((double)rand() / RAND_MAX * DIM)};
     cout << "(" << p.first << "," << p.second << ") ";
     */
    uniq.insert(p);
  }

  for (auto it = uniq.begin(); it != uniq.end(); it++) {
    samples.push_back(*it);
  }

  KDTree *kd = kd_init(samples, 0);
  vector<pair<double, Point>> heap;

  Rec cell({0, 0}, {DIM, DIM});
  for (auto p : samples) {
    vector<Point> within_range;
    Rec range({p.first - R_MAX, p.second - R_MAX},
              {p.first + R_MAX, p.second + R_MAX});

    range_search(range, kd, cell, within_range);
    /*
    for (Point s : samples) {
      if (contains_point(range, s))
        within_range.push_back(s);
    }
    */
    double sum_w = 0;
    for (auto s : within_range) {
      double dist = (p.first - s.first) * (p.first - s.first) +
                    (s.second - p.second) * (s.second - p.second);
      dist = sqrt(dist);
      //  artificially increases weight for white areas
      dist = dist * (3 - (2 * (gradient[p.first][p.second]) / 255));
      /*
      cout << "orig dist: " << dist << " r_max: " << R_MAX
           << " r_min: " << R_MIN << " ";
      */
      dist = min(dist, 2 * R_MAX);
      dist = dist > 2 * R_MIN ? dist : 2 * R_MIN;

      // large distances lead to larger weights
      double apply_sparsing = dist == 2 * R_MIN ? 2 : 1;
      double w = pow(apply_sparsing - (dist / (2 * R_MAX)), ALPHA);
      /*
      cout << "Point: " << p.first << "," << p.second << " dist: " << dist
           << " weight: " << w << " density: " << gradient[p.first][p.second]
           << endl;
      */
      sum_w += w;
    }
    // cout << "final weight: " << sum_w << endl;

    pair<double, Point> heap_node = {sum_w, p};
    heap.push_back(heap_node);
  }

  sort(heap.begin(), heap.end());

  /*
  for (int i = 0; i < heap.size(); i++) {
    auto wp = heap[i];
    Point p = wp.second;
    double weight = wp.first;
    cout << "weight: " << weight << " ";
    cout << "(" << p.first << "," << p.second << ") ";
    cout << "index: " << i << endl;
  }
  */

  // store index of point in heap
  map<Point, int> dict;
  for (int i = 0; i < heap.size(); i++) {
    Point p = heap[i].second;
    dict[p] = i;
  }

  set<Point> deleted;
  while (heap.size() > N) {
    auto wp = heap.back();
    Point p = wp.second;
    deleted.insert(p);
    delete_node(kd, p);
    vector<Point> within_range(0);
    Rec range({p.first - R_MAX, p.second - R_MAX},
              {p.first + R_MAX, p.second + R_MAX});

    range_search(range, kd, cell, within_range);
    /*
    for (Point s : samples) {
      if (contains_point(range, s) && deleted.find(s) == deleted.end())
        within_range.push_back(s);
    }
    */
    for (int i = 0; i < heap.size(); i++) {
      auto wp = heap[i];
      Point p = wp.second;
      double weight = wp.first;

      // cout << "weight: " << weight << " ";
      // cout << "(" << p.first << "," << p.second << ") ";
      // cout << "index: " << i << endl;
    }
    /*
    cout << "deleted: ";
    for (auto x : deleted) {
      cout << "(" << x.first << "," << x.second << ") ";
    }
    cout << endl;
    */
    // cout << "======================" << endl;
    for (auto s : within_range) {

      // cout << "looking into " << s.first << "," << s.second << endl;
      if (deleted.find(s) != deleted.end()) {
        continue;
        // cout << "deleted node found in kd" << endl;
      }

      double dist = (s.first - p.first) * (s.first - p.first) +
                    (s.second - p.second) * (s.second - p.second);
      dist = sqrt(dist);
      dist = dist * (3 - (2 * (gradient[p.first][p.second]) / 255));
      dist = min(dist, 2 * R_MAX);
      dist = dist > 2 * R_MIN ? dist : 2 * R_MIN;
      double apply_sparsing = dist == 2 * R_MIN ? 2 : 1;
      double w = pow(apply_sparsing - (dist / (2 * R_MAX)), ALPHA);
      int ind = dict[s];
      if (ind < 0)
        continue;
      /*
      if (heap[ind].second != s) {
        cout << "GETTING INDEX WAS WRONG index: " << ind << " got "
             << heap[ind].second.first << "," << heap[ind].second.second
             << endl;
      }
      */

      heap[ind].first -= w;
    }

    heap.pop_back();

    // cout << endl;
    sort(heap.begin(), heap.end());
    dict[p] = -1;
    // update index of point in heap
    for (int i = 0; i < heap.size(); ++i) {
      Point e = heap[i].second;
      dict[e] = i;
    }
  }
  cout << "TEST " << 0 << endl;

  vector<vector<int>> img(DIM, vector<int>(DIM, 255));
  while (heap.size() > 0) {
    auto wp = heap.front();
    pop_heap(heap.begin(), heap.end());
    heap.pop_back();

    Point p = wp.second;
    img[p.first][p.second] = 0;
  }

  unsigned char *data = (unsigned char *)malloc(DIM * DIM);
  for (int i = 0; i < img.size(); i++) {
    for (int j = 0; j < img[0].size(); j++) {
      data[i * DIM + j] = img[i][j];
      // cout << img[i][j] << " ";
    }
    // cout << endl;
  }

  unsigned char *gr_data = (unsigned char *)malloc(DIM * DIM);
  for (int i = 0; i < img.size(); i++) {
    for (int j = 0; j < img[0].size(); j++) {
      gr_data[i * DIM + j] = gradient[i][j];
      // cout << img[i][j] << " ";
    }
    // cout << endl;
  }

  string filename = "adaptive_test.jpg";
  string gr_name = "gradient.jpg";
  // stbi_write_jpg(gr_name.c_str(), DIM, DIM, 1, gradient, 100);
  stbi_write_jpg(filename.c_str(), DIM, DIM, 1, data, 100);
  stbi_write_jpg(gr_name.c_str(), DIM, DIM, 1, gr_data, 100);
  return 0;
}
