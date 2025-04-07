#include <random>
#include <set>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
using namespace std;

typedef pair<int, int> Point;
int main() {
  const int DIM = 256;
  const int M = 3000;
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

  unsigned char *random = (unsigned char *)malloc(DIM * DIM);
  for (int i = 0; i < DIM; i++) {
    for (int j = 0; j < DIM; j++) {
      random[i * DIM + j] = 255;
    }
  }

  for (int i = 0; i < samples.size(); i++) {
    random[(int)samples[i].first * DIM + (int)samples[i].second] = 0;
  }

  string r_name = "random.jpg";
  stbi_write_jpg(r_name.c_str(), DIM, DIM, 1, random, 100);
}
