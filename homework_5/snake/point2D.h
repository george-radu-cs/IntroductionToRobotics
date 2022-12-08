/**
 * File that contains a struct for a point in a 2D space with the definition of the equality operator (==)
 */

#ifndef STRUCTS_H
#define STRUCTS_H

struct Point2D {
  byte x = 0;
  byte y = 0;

  Point2D(byte x = 0, byte y = 0)
      : x(x), y(y) {}
};

bool operator==(const Point2D &lhs, const Point2D &rhs) {
  return lhs.x == rhs.x && lhs.y == rhs.y;
}

#endif