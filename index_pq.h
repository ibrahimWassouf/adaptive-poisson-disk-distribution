#ifndef INDEX_PQ_H
#define INDEX_PQ_H

#include <map>
#include <utility>
#include <vector>

typedef std::pair<int, int> Point;

class IndexPQ {
public:
  int counter = 0;
  std::map<Point, int> keys;
  std::vector<std::pair<double, Point>> pq;
  std::vector<int> qp;

  int size() { return pq.size(); }
  void exchange(int, int, int, int);
  void sink(int);
  void swim(int);
  void push(std::pair<double, Point>);
  std::pair<double, Point> pop();
  void change(int, double);
};

#endif
