#include "rec.h"

using namespace std;

bool contains_point(Rec &r, Point &p) {
  if ((r.bottom_left.x <= p.x && p.x <= r.top_right.x) &&
      (r.bottom_left.y <= p.y && p.y <= r.top_right.y))
    return true;
  return false;
}

bool contains_cell(Rec &r, Rec &c) {

  if (r.top_right.x >= c.top_right.x && r.top_right.y >= c.top_right.y &&
      r.bottom_left.x <= c.bottom_left.x && r.bottom_left.y <= c.bottom_left.y)
    return true;
  return false;
}

bool is_disjoint(Rec &r, Rec &c) {
  if (r.top_right.x < c.bottom_left.x || r.top_right.y < c.bottom_left.y ||
      r.bottom_left.x > c.top_right.x || r.bottom_left.y > c.top_right.y)
    return true;
  return false;
}

Rec left_rec(Rec &r, int axis, Point &median) {
  if (axis == 0) {
    Rec left(r.bottom_left, {median.x, r.top_right.y});
    return left;
  }

  Rec left(r.bottom_left, {r.top_right.x, median.y});
  return left;
}

Rec right_rec(Rec &r, int axis, Point &median) {
  if (axis == 0) {
    Rec right({median.x, r.bottom_left.y}, r.top_right);
    return right;
  }
  Rec right({r.bottom_left.x, median.y}, r.top_right);
  return right;
}
