#ifndef ANT_MATH_H
#define ANT_MATH_H

typedef struct {
  int x;
  int y;
} Position;

typedef struct {
  double x;
  double y;
} AntVector;

float lerp(float a, float b, float t);
float smoothstep(float t);
float noise2d(float x, float y, int seed);

#endif // ANT_MATH_H