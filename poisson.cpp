#include "kd-tree.h"
#include "rec.h"
#include <algorithm>
#include <iostream>
#include <map>
#include <queue>
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
  seed_seq seed{r(), r(), r(), r()};
  mt19937 e1;
  e1.seed(seed);
  uniform_int_distribution<int> uniform_dist(0, 63);

  const int M = 2000;
  const int N = 50;

  // create random samples
  vector<Point> samples;
  set<Point> uniq;
  while (uniq.size() < M) {
    Point p = {uniform_dist(e1), uniform_dist(e1)};
    uniq.insert(p);
  }

  for (auto it = uniq.begin(); it != uniq.end(); it++) {
    samples.push_back(*it);
  }

  KDTree *kd = kd_init(samples, 0);

  const double AREA = 64 * 64;
  const double R_MAX = sqrt(AREA / (2.0 * sqrt(3.0) * N));
  const double R_MIN = R_MAX * (1 - pow(N / M, 1.5)) * 0.65;

  vector<pair<double, Point>> heap;

  Rec cell({0, 0}, {64, 64});
  for (auto p : samples) {
    vector<Point> within_range;
    Rec range({p.first - R_MAX, p.second - R_MAX},
              {p.first + R_MAX, p.second + R_MAX});

    range_search(range, kd, cell, within_range);
    double sum_w = 0;
    for (auto s : within_range) {
      double dist = (p.first - s.first) * (p.first - s.first) +
                    (s.second - p.second) * (s.second - p.second);
      dist = min(sqrt(dist), 2 * R_MAX);
      double w = pow(1 - (dist / (2 * R_MAX)), 7);
      sum_w += w;
    }

    pair<double, Point> heap_node = {sum_w, p};
    heap.push_back(heap_node);
  }

  make_heap(heap.begin(), heap.end());
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

  while (heap.size() > N) {
    auto wp = heap.front();
    Point p = wp.second;
    vector<Point> within_range;
    Rec range({p.first - R_MAX, p.second - R_MAX},
              {p.first + R_MAX, p.second + R_MAX});

    range_search(range, kd, cell, within_range);
    for (int i = 0; i < heap.size(); i++) {
      auto wp = heap[i];
      Point p = wp.second;
      double weight = wp.first;
      cout << "weight: " << weight << " ";
      cout << "(" << p.first << "," << p.second << ") ";
      cout << "index: " << i << endl;
    }
    cout << "======================" << endl;
    for (auto s : within_range) {
      cout << "looking into " << s.first << "," << s.second << endl;
      double dist = (s.first - p.first) * (s.first - p.first) +
                    (s.second - p.second) * (s.second - p.second);
      dist = min(sqrt(dist), 2 * R_MAX);
      dist = dist > 2 * R_MIN ? dist : 2 * R_MIN;
      double w = pow(1 - (dist / (2 * R_MAX)), 7);
      int ind = dict[s];
      if (heap[ind].second != s) {
        cout << "GETTING INDEX WAS WRONG index: " << ind << " got "
             << heap[ind].second.first << "," << heap[ind].second.second
             << endl;
      }
      heap[ind].first -= w;
    }

    pop_heap(heap.begin(), heap.end());
    heap.pop_back();

    cout << endl;
    make_heap(heap.begin(), heap.end());

    // update index of point in heap
    for (int i = 0; i < heap.size(); i++) {
      Point p = heap[i].second;
      dict[p] = i;
    }
    delete_node(kd, p);
  }
  /*
    cout << "HEAP IN END ==========" << endl;
    cout << heap.size() << endl;
    for (auto x : heap) {
      cout << x.first << " ";
      cout << "(" << x.second.first << "," << x.second.second << ") ";
    }
    */
  //====================================
  // old code - start
  //====================================
  // using unique random samples.
  /*
  set<Point> random_samples;
  while (random_samples.size() < M) {
    Point p = {uniform_dist(e1), uniform_dist(e1)};
    random_samples.insert(p);
  }
  cout << random_samples.size() << endl;
  vector<Point> samples(random_samples.begin(), random_samples.end());
  */

  // using random generation only
  /*
    vector<Point> samples;
    for (int i = 0; i < M; i++) {

      Point p = {uniform_dist(e1), uniform_dist(e1)};
      samples.push_back(p);
    }

    /*
    for (auto p : samples) {
      cout << "(" << p.first << "," << p.second << ") ";
    }
    */
  /*
    KDTree *kd = kd_init(samples, 0);

    // cout << "kd tree created\n";
    const double AREA = 64 * 64;
    const double R_MAX = ceil(sqrt(AREA / (2.0 * sqrt(3.0) * N)));

    // kd cell of entire ouput image
    Rec cell({0, 0}, {64, 64});

    multimap<double,Point> heap;
    map<Point, double> dict;

    // calculate weight of each sample
    for (auto p : samples) {
      vector<Point> within_range;
      Rec range({p.first - R_MAX, p.second - R_MAX},
                {p.first + R_MAX, p.second + R_MAX});

      // cout << "finding samples within range\n";
      range_search(range, kd, cell, within_range);
      double sum_w = 0;
      for (auto s : within_range) {
        double dist = (s.first - p.first) * (s.first - p.first) +
                      (s.second - p.second) * (s.second - p.second);
        dist = min(sqrt(dist), 2 * R_MAX);
        double w = pow(1 - (dist / (2 * R_MAX)), 8);
        // cout << w << endl;
        sum_w += w;
      }

      /*
      cout << "r max: " << R_MAX << endl;
      cout << "Total weight: " << sum_w << endl;
      cout << "sample point: " << p.first << "," << p.second << endl;
      for (auto x : within_range) {
        cout << "(" << x.first << "," << x.second << ") ";
      }
      cout << endl;
      */
  /*
    pair<double, Point> heap_node = {sum_w, p};
    // cout << "inserting into heap\n";
    heap.push(heap_node);
    dict[p] = sum_w;
  }
  */
  /*
    cout << "heap size start: " << heap.size() << endl;
    set<Point> deleted;
    map<Point, int> times;
    while (deleted.size() < M - N && !heap.empty()) {

      // get the point with highest weight and remove it from the samples
      // and subtract it's weight from neighboring samples.
      auto weight_and_p = heap.top();
      heap.pop();

      double weight = weight_and_p.first;
      Point p = weight_and_p.second;

      times[p] += 1;
      deleted.insert(p);
  */
  // cout << heap.size() << " top: " << p.first << "," << p.second
  //     << " weight: " << weight << " dict weight: " << dict[p] << endl;

  // if (heap.size() >= 10096)
  // break;
  // indicates an outdated weight
  /*
      if (dict[p] != weight || dict[p] <= -1)
        continue;

      cout << "(" << p.first << "," << p.second << ") times: " << times[p]
           << endl;
      vector<Point> within_range;

      Rec range({p.first - R_MAX, p.second - R_MAX},
                {p.first + R_MAX, p.second + R_MAX});

      range_search(range, kd, cell, within_range);

      for (auto x : within_range) {
        // grab iterator position of neighboring sample in the heap
        auto old_weight = dict[x];

        // update weight of neighboring sample in heap
        double dist = (x.first - p.first) * (x.first - p.first) +
                      (x.second - p.second) * (x.second - p.second);

        dist = min(sqrt(dist), 2 * R_MAX);
        double w = pow(1 - (dist / (2 * R_MAX)), 8);
        double new_weight = old_weight - w;
        cout << "new_weight: " << new_weight << endl;
        dict[x] = new_weight;
        heap.push({new_weight, x});
      }

      delete_node(kd, p);
      dict[p] = -1;
    }

    cout << "heap size end: " << heap.size() << endl;
    cout << "deleted: " << deleted.size() << endl;
  */
  // using heap only
  /*
  set<Point> s;
  vector<vector<int>> img(64, vector<int>(64, 0));
  while (!heap.empty()) {
    auto wp = heap.top();
    heap.pop();
    Point p = wp.second;

    // if (dict[p] != wp.first)
    //  continue;
    //  cout << p.first << "," << p.second << endl;
    s.insert(p);
    img[p.first][p.second] = 255;
  }
  cout << s.size() << endl;
  */
  // using deleted set
  /*
    vector<vector<int>> img(64, vector<int>(64, 0));
    for (Point x : samples) {
      if (deleted.find(x) == deleted.end()) {
        img[x.first][x.second] = 255;

        cout << "(" << x.first << "," << x.second << ") ";
      }
    }
  */
  //====================================
  // old code - end
  //====================================

  vector<vector<int>> img(64, vector<int>(64, 0));
  while (heap.size() > 0) {
    auto wp = heap.front();
    pop_heap(heap.begin(), heap.end());
    heap.pop_back();

    Point p = wp.second;
    img[p.first][p.second] = 255;
  }

  unsigned char *data = (unsigned char *)malloc(64 * 64);
  for (int i = 0; i < img.size(); i++) {
    for (int j = 0; j < img[0].size(); j++) {
      data[i * 64 + j] = img[i][j];
      // cout << img[i][j] << " ";
    }
    // cout << endl;
  }

  string filename = "test.jpg";
  stbi_write_jpg(filename.c_str(), 64, 64, 1, data, 100);
  return 0;
}
