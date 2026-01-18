#ifndef ANT_MATH_H
#define ANT_MATH_H

#include <stdbool.h>
typedef struct {
  int x;
  int y;
} Position;

typedef struct {
  double x;
  double y;
} AntVector;

bool Position_equals(Position p1, Position p2);
AntVector AntVector_from_positions(Position from, Position to);
AntVector AntVector_normalize(AntVector v);
AntVector AntVector_scale(AntVector v, float scalar);
AntVector AntVector_add(AntVector v1, AntVector v2);
AntVector AntVector_add_inverse(AntVector v);

int hash(int x, int y, int seed);
float lerp(float a, float b, float t);
float smoothstep(float t);
float noise2d(float x, float y, int seed);
float dist_sq(Position p1, Position p2);

#endif // ANT_MATH_H