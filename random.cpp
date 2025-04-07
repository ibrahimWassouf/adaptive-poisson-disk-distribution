#include "index_pq.h"
#include "kd-tree.h"
#include "rec.h"
#include <algorithm>
#include <map>
#include <random>
#include <set>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace std;

int main(int argc, char *argv[]) {
  const int DIM = 256;
  const double M = (double)trunc(atoi(argv[1]));
  const double N = (double)trunc(atoi(argv[2]));
  const double ALPHA = 8;
  const double GAMMA = 1.5;
  const double BETA = 0.65;

  const double AREA = DIM * DIM;
  const double R_MAX_2D = sqrt(AREA / (2.0 * sqrt(3.0) * N));
  const double R_MAX = 3 * R_MAX_2D;
  const double R_MIN = R_MAX / 3 * (1 - pow(N / M, GAMMA)) * BETA;

  random_device r;
  seed_seq seed{r(), r(), r(), r()};
  mt19937 e1;
  e1.seed(seed);
  uniform_int_distribution<int> uniform_dist(0, DIM - 1);
  // create random samples
  vector<Point> samples;
  set<Point> uniq;
  while (uniq.size() < N) {
    int x = uniform_dist(e1);
    int y = uniform_dist(e1);
    Point p{(double)x, (double)y};
    uniq.insert(p);
  }

  for (auto it = uniq.begin(); it != uniq.end(); it++) {
    samples.push_back(*it);
  }

  unsigned char *random = (unsigned char *)malloc(DIM * DIM);
  for (int i = 0; i < DIM; i++) {
    for (int j = 0; j < DIM; j++) {
      random[i * DIM + j] = 255;
    }
  }

  // data representing final image of random stipples
  for (int i = 0; i < samples.size(); i++) {
    random[(int)samples[i].first * DIM + (int)samples[i].second] = 0;
  }

  // generate rest of sample set for pdd
  uniq.clear();
  while (uniq.size() < M) {
    int x = uniform_dist(e1);
    int y = uniform_dist(e1);
    Point p{(double)x, (double)y};
    uniq.insert(p);
  }

  samples.clear();
  for (auto it = uniq.begin(); it != uniq.end(); it++) {
    samples.push_back(*it);
  }

  // generate data for pdd image
  KDTree *kd = kd_init(samples, 0);

  IndexPQ heap;

  Rec cell({0, 0}, {(double)DIM, (double)DIM});
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

      dist = min(dist, 2 * R_MAX);
      dist = dist > 2 * R_MIN ? dist : 2 * R_MIN;

      // large distances lead to smaller weights
      int white_disc = dist == 2 * R_MIN ? 7 : 1;
      double w = pow(white_disc - (dist / (2 * R_MAX)), ALPHA - white_disc);

      sum_w += w;
    }

    pair<double, Point> heap_node = {sum_w, p};
    heap.push(heap_node);
  }

  set<Point> deleted;
  while (heap.size() > N) {
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
      if (ind < 0)
        continue;
      double dist = (s.first - p.first) * (s.first - p.first) +
                    (s.second - p.second) * (s.second - p.second);
      dist = sqrt(dist);
      dist = min(dist, 2 * R_MAX);
      dist = dist > 2 * R_MIN ? dist : 2 * R_MIN;

      int white_disc = dist == 2 * R_MIN ? 7 : 1;
      double w = pow(white_disc - (dist / (2 * R_MAX)), ALPHA - white_disc);
      int s_key = heap.keys[s];
      int s_pq_index = heap.qp[s_key];
      double sum_w = heap.pq[s_pq_index].first;
      sum_w -= w;
      heap.change(s_key, sum_w);
    }
  }

  vector<vector<int>> pdd(DIM, vector<int>(DIM, 255));
  for (auto wp : heap.pq) {
    Point p = wp.second;
    pdd[p.first][p.second] = 0;
  }

  unsigned char *data = (unsigned char *)malloc(DIM * DIM);
  for (int i = 0; i < pdd.size(); i++) {
    for (int j = 0; j < pdd[0].size(); j++) {
      data[i * DIM + j] = pdd[i][j];
    }
  }

  string stipples = argv[2];
  string output_filename = "./output/pdd_" + stipples + ".jpg";
  stbi_write_jpg(output_filename.c_str(), DIM, DIM, 1, data, 100);

  string r_name = "./output/random_" + stipples + ".jpg";
  stbi_write_jpg(r_name.c_str(), DIM, DIM, 1, random, 100);
}
