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

string file_from_path(string path) {
  string file = "";
  for (char c : path) {
    if (c == '/') {
      file = "";
    } else
      file += c;
  }
  return file;
}

string remove_extension(string filename) {
  string res = "";
  for (char c : filename) {
    if (c == '.')
      break;
    res += c;
  }
  return res;
}

int main(int argc, char *argv[]) {

  if (argc != 4) {
    cout << "Please input filename, sample size (M), and subset sample size "
            "(N) as input\n";
    return 1;
  }

  string image_path = argv[1];
  int x, y, n, ok;
  ok = stbi_info(image_path.c_str(), &x, &y, &n);
  if (!ok) {
    cout << "File: " << argv[1] << " file format not supported\n";
    return 1;
  }

  unsigned char *gr_char = stbi_load(image_path.c_str(), &x, &y, &n, 0);

  const double M = (double)trunc(atoi(argv[2]));
  const double N = (double)trunc(atoi(argv[3]));
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

  auto start = chrono::high_resolution_clock::now();
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
    Point p{(double)x, (double)y};
    uniq.insert(p);
  }

  for (auto it = uniq.begin(); it != uniq.end(); it++) {
    samples.push_back(*it);
  }

  KDTree *kd = kd_init(samples, 0);

  IndexPQ heap;

  // vector<pair<double, Point>> heap;
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
      //  artificially increases weight for white areas
      dist = dist * (3 - (2 * (gradient[p.first][p.second]) / 255));

      dist = min(dist, 2 * R_MAX);
      dist = dist > 2 * R_MIN ? dist : 2 * R_MIN;

      // large distances lead to smaller weights
      int white_disc = dist == 2 * R_MIN ? 7 : 1;
      double w = pow(white_disc - (dist / (2 * R_MAX)), ALPHA - white_disc);

      sum_w += w;
    }

    pair<double, Point> heap_node = {sum_w, p};
    // heap.push_back(heap_node);
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
    }
  }

  auto stop = chrono::high_resolution_clock::now();
  auto time = chrono::duration_cast<chrono::milliseconds>(stop - start).count();
  vector<vector<int>> img(DIM, vector<int>(DIM, 255));

  string filename = file_from_path(image_path);
  filename = remove_extension(filename);
  string result = filename + " " + to_string(M) + " " + to_string(N) + " " +
                  to_string(time) + "\n";
  cout << filename << " " << M << " " << N << " " << time << endl;
  string output_data = "./data/" + filename + "_" + argv[3] + ".txt";
  ofstream file;
  file.open(output_data);

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

  string output_filename = "./output/" + filename + "_" + argv[3] + ".jpg";
  stbi_write_jpg(output_filename.c_str(), DIM, DIM, 1, data, 100);
  return 0;
}
