#ifndef ANT_MATH_H
#define ANT_MATH_H

#include <stdbool.h>

typedef struct {
  double x;
  double y;
} AntVector;

AntVector AntVector_normalize(AntVector v);
AntVector AntVector_scale(AntVector v, float scalar);
AntVector AntVector_add(AntVector v1, AntVector v2);
AntVector AntVector_reverse(AntVector v);

int hash(int x, int y, int seed);
float lerp(float a, float b, float t);
float smoothstep(float t);
float noise2d(float x, float y, int seed);
float dist_sq(AntVector p1, AntVector p2);

#endif // ANT_MATH_H