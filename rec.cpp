#include "rec.h"
#include <iostream>
bool contains_point(double radius, Point &c, Point &p) {
  double a = (p.first - c.first)*(p.first - c.first);
  double b = (p.second - c.second)*(p.second - c.second);  
  double dist = a + b;
                
    
  //std::cout << a << " " << b << "\n"; 
  return dist <= radius*radius;
}

bool contains_cell(Rec &r, Rec &c) {

  if (r.top_right.first >= c.top_right.first &&
      r.top_right.second >= c.top_right.second &&
      r.bottom_left.first <= c.bottom_left.first &&
      r.bottom_left.second <= c.bottom_left.second)
    return true;
  return false;
}

bool is_disjoint(Rec &r, Rec &c) {
  if ((r.top_right.first < c.bottom_left.first ||
       r.top_right.second < c.bottom_left.second) ||
      (r.bottom_left.first > c.top_right.first ||
       r.bottom_left.second > c.top_right.second))
    return true;
  return false;
}

Rec left_rec(Rec &r, int axis, Point &median) {
  if (axis == 0) {
    Rec left(r.bottom_left, {median.first, r.top_right.second});
    return left;
  }

  Rec left(r.bottom_left, {r.top_right.first, median.second});
  return left;
}

Rec right_rec(Rec &r, int axis, Point &median) {
  if (axis == 0) {
    Rec right({median.first, r.bottom_left.second}, r.top_right);
    return right;
  }
  Rec right({r.bottom_left.first, median.second}, r.top_right);
  return right;
}
