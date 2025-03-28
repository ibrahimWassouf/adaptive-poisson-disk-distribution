#include "index_pq.h"
#include "kd-tree.h"
#include "rec.h"
#include <algorithm>
#include <chrono>
#include <fstream>
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

int main() {

  int x, y, n, ok;

  ok = stbi_info("gray_test.jpg", &x, &y, &n);
  if (!ok) {
    cout << "file format not supported\n";
    return 1;
  }

  unsigned char *gr_char = stbi_load("gray_test.jpg", &x, &y, &n, 0);

  const double M = 30000;
  const double N = 10000;
  const double ALPHA = 8;
  const double GAMMA = 1.5;
  const double BETA = 0.65;

  const int DIM = max(x, y);
  const double AREA = DIM * DIM;
  const double R_MAX_2D = sqrt(AREA / (2.0 * sqrt(3.0) * N));
  const double R_MAX = 3 * R_MAX_2D;
  const double R_MIN = R_MAX / 3 * (1 - pow(N / M, GAMMA)) * BETA;

  vector<vector<double>> gradient(DIM, vector<double>(DIM, 255));
  for (int i = 0; i < x; i++) {
    for (int j = 0; j < y; j++) {
      gradient[i][j] = gr_char[i * x + j];
    }
  }

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

    uniq.insert(p);
  }

  for (auto it = uniq.begin(); it != uniq.end(); it++) {
    samples.push_back(*it);
  }

  KDTree *kd = kd_init(samples, 0);

  IndexPQ heap;

  // vector<pair<double, Point>> heap;
  Rec cell({0, 0}, {DIM, DIM});
  for (auto p : samples) {
    vector<Point> within_range;
    Rec range({p.first - R_MAX, p.second - R_MAX},
              {p.first + R_MAX, p.second + R_MAX});

    for (Point x : samples) {
      if (contains_point(range, x))
        within_range.push_back(x);
    }
    double sum_w = 0;
    for (auto s : within_range) {
      double dist = (p.first - s.first) * (p.first - s.first) +
                    (s.second - p.second) * (s.second - p.second);
      dist = sqrt(dist);
      //  artificially increases weight for white areas
      dist = dist * (3 - (2 * (gradient[p.first][p.second]) / 255));

      dist = min(dist, 2 * R_MAX);
      dist = dist > 2 * R_MIN ? dist : 2 * R_MIN;

      // large distances lead to larger weights
      int white_disc = dist == 2 * R_MIN ? 7 : 1;
      double w = pow(white_disc - (dist / (2 * R_MAX)), ALPHA - white_disc);

      sum_w += w;
    }

    pair<double, Point> heap_node = {sum_w, p};
    // heap.push_back(heap_node);
    heap.push(heap_node);
  }

  /*
  sort(heap.begin(), heap.end());
  map<Point, int> dict;
  for (int i = 0; i < heap.size(); i++) {
    Point p = heap[i].second;
    dict[p] = i;
  }
  */
  set<Point> deleted;
  while (heap.size() > N) {
    /*
    auto wp = heap.back();
    Point p = wp.second;
    deleted.insert(p);
    delete_node(kd, p);
    */
    auto wp = heap.pop();
    Point p = wp.second;
    delete_node(kd, p);
    deleted.insert(p);

    vector<Point> within_range(0);
    Rec range({p.first - R_MAX, p.second - R_MAX},
              {p.first + R_MAX, p.second + R_MAX});

    range_search(range, kd, cell, within_range);

    for (auto s : within_range) {
      if (deleted.find(s) != deleted.end()) {
        continue;
      }
      int ind = heap.keys[s];
      /*
      int ind = dict[s];
      */
      if (ind < 0)
        continue;
      double dist = (s.first - p.first) * (s.first - p.first) +
                    (s.second - p.second) * (s.second - p.second);
      dist = sqrt(dist);
      dist = dist * (3 - (2 * (gradient[p.first][p.second]) / 255));
      dist = min(dist, 2 * R_MAX);
      dist = dist > 2 * R_MIN ? dist : 2 * R_MIN;

      int white_disc = dist == 2 * R_MIN ? 7 : 1;
      double w = pow(white_disc - (dist / (2 * R_MAX)), ALPHA - white_disc);
      int s_key = heap.keys[s];
      int s_pq_index = heap.qp[s_key];
      double sum_w = heap.pq[s_pq_index].first;
      sum_w -= w;
      heap.change(s_key, sum_w);
      // heap[ind].first -= w;
    }
    /*
    heap.pop_back();


    // cout << endl;
    sort(heap.begin(), heap.end());
    dict[p] = -1;
    // update index of point in heap
    for (int i = 0; i < heap.size(); ++i) {
      Point e = heap[i].second;
      dict[e] = i;
    }
    */
  }

  vector<vector<int>> img(DIM, vector<int>(DIM, 255));

  /*
  while (heap.size() > 0) {
    auto wp = heap.front();
    pop_heap(heap.begin(), heap.end());
    heap.pop_back();

    Point p = wp.second;
    img[p.first][p.second] = 0;
  }
  */
  ofstream file;
  file.open("data.txt");

  for (auto wp : heap.pq) {
    Point p = wp.second;
    img[p.first][p.second] = 0;
    file << p.first << " " << p.second << '\n';
  }
  file.close();
  unsigned char *data = (unsigned char *)malloc(DIM * DIM);
  for (int i = 0; i < img.size(); i++) {
    for (int j = 0; j < img[0].size(); j++) {
      data[i * DIM + j] = img[i][j];
    }
  }

  unsigned char *gr_data = (unsigned char *)malloc(DIM * DIM);
  for (int i = 0; i < img.size(); i++) {
    for (int j = 0; j < img[0].size(); j++) {
      gr_data[i * DIM + j] = gradient[i][j];
    }
  }

  string filename = "adaptive_test.jpg";
  string gr_name = "gradient.jpg";
  // stbi_write_jpg(gr_name.c_str(), DIM, DIM, 1, gradient, 100);
  stbi_write_jpg(filename.c_str(), DIM, DIM, 1, data, 100);
  stbi_write_jpg(gr_name.c_str(), DIM, DIM, 1, gr_data, 100);
  return 0;
}
