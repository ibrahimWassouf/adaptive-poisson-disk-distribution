#ifndef REC_H
#define REC_H
#include <utility>

typedef std::pair<int, int> Point;

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
bool contains_point(Rec &, Point &);
bool is_disjoint(Rec &, Rec &);
Rec left_rec(Rec &, int, Point &);
Rec right_rec(Rec &, int, Point &);
#endif
