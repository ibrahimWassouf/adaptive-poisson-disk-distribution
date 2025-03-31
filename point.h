#ifndef POINT_H
#define POINT_H
struct Point {
  double first;
  double second;
  bool operator<(const Point &y) const {
    if (first == y.first)
      return second < y.second;
    return first < y.first;
  };
  bool operator<=(const Point &y) const {
    if (first == y.first)
      return second <= y.second;
    return first <= y.first;
  };
  bool operator>(const Point &y) const {
    if (first == y.first)
      return second > y.second;
    return first > y.first;
  };
  bool operator>=(const Point &y) const {
    if (first == y.first)
      return second >= y.second;
    return first >= y.first;
  };
  bool operator==(const Point &y) const {
    if (first == y.first)
      return second == y.second;
    return first == y.first;
  };
  bool operator!=(const Point &y) const {
    if (first == y.first)
      return second != y.second;
    return first != y.first;
  };
};
#endif
