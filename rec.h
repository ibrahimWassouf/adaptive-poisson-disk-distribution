#ifndef REC_H
#define REC_H
#include "point.h"
#include <utility>

class Rec {
public:
  Point bottom_left;
  Point top_right;

  Rec(Point bl, Point tr) {
    bottom_left = bl;
    top_right = tr;
  };
};

bool contains_cell(Rec &, Rec &);
bool contains_point(double radius, Point &, Point &);
bool is_disjoint(Rec &, Rec &);
Rec left_rec(Rec &, int, Point &);
Rec right_rec(Rec &, int, Point &);
#endif
