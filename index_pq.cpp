#include "index_pq.h"

using namespace std;

void IndexPQ::exchange(int i, int j, int keyi, int keyj) {

  pair<double, Point> swap = pq[i];
  pq[i] = pq[j];
  pq[j] = swap;

  // update position of element with associated key in the pq
  qp[keyi] = j;
  qp[keyj] = i;
}

void IndexPQ::sink(int i) {
  while (i < pq.size()) {
    int j = i;
    if ((2 * i + 1) < pq.size() && pq[2 * i + 1].first > pq[j].first)
      j = 2 * i + 1;
    if ((2 * i + 2) < pq.size() && pq[2 * i + 2].first > pq[j].first) {
      j += 1;
    }
    if (i != j) {
      exchange(i, j, keys[pq[i].second], keys[pq[j].second]);
      i = j;
    } else
      break;
  }
}

void IndexPQ::swim(int i) {
  while (i > 0) {
    int j = (i - 1) / 2;
    if (pq[i] > pq[j]) {
      exchange(i, j, keys[pq[i].second], keys[pq[j].second]);
      i = j;
    } else
      break;
  }
}

void IndexPQ::push(pair<double, Point> el) {
  pq.push_back(el);
  keys[el.second] = counter;
  qp.push_back(pq.size() - 1);
  counter += 1;
  swim(pq.size() - 1);
}

pair<double, Point> IndexPQ::pop() {
  pair<double, Point> top = pq[0];
  Point p1 = top.second;
  Point p2 = pq[pq.size() - 1].second;
  exchange(0, pq.size() - 1, keys[p1], keys[p2]);
  // item with this key no longer in pq
  qp[keys[p1]] = -1;
  pq.pop_back();
  sink(0);

  return top;
}

void IndexPQ::change(int key, double w) {
  int pq_index = qp[key];
  pq[pq_index].first = w;
  swim(pq_index);
  sink(pq_index);
}
