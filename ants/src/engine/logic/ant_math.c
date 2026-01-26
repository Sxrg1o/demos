#include "ant_math.h"
#include <math.h>
#include <stdint.h>

AntVector AntVector_normalize(AntVector v) {
  double len = sqrt(v.x * v.x + v.y * v.y);
  if (len == 0.0)
    return (AntVector){0.0, 0.0};
  return (AntVector){v.x / len, v.y / len};
}

AntVector AntVector_scale(AntVector v, float scalar) {
  return (AntVector){v.x * (double)scalar, v.y * (double)scalar};
}

AntVector AntVector_add(AntVector v1, AntVector v2) {
  return (AntVector){v1.x + v2.x, v1.y + v2.y};
}

AntVector AntVector_reverse(AntVector v) { return (AntVector){-v.x, -v.y}; }

int hash(int x, int y, int seed) {
  int h = seed + x * 374761393 + y * 668265263;
  h = (h ^ (h >> 13)) * 1274126177;
  return h ^ (h >> 16);
}

float lerp(float a, float b, float t) { return a + t * (b - a); }

float smoothstep(float t) { return t * t * (3.0f - 2.0f * t); }

float noise2d(float x, float y, int seed) {
  int xi = (int)floorf(x);
  int yi = (int)floorf(y);

  float tx = x - xi;
  float ty = y - yi;

  float sx = smoothstep(tx);
  float sy = smoothstep(ty);

  // Hash corners
  float n00 = (hash(xi, yi, seed) & 0xFFFF) / 65535.0f;
  float n10 = (hash(xi + 1, yi, seed) & 0xFFFF) / 65535.0f;
  float n01 = (hash(xi, yi + 1, seed) & 0xFFFF) / 65535.0f;
  float n11 = (hash(xi + 1, yi + 1, seed) & 0xFFFF) / 65535.0f;

  float nx0 = lerp(n00, n10, sx);
  float nx1 = lerp(n01, n11, sx);

  return lerp(nx0, nx1, sy);
}

float dist_sq(AntVector p1, AntVector p2) {
  int dx = p1.x - p2.x;
  int dy = p1.y - p2.y;
  return (float)(dx * dx + dy * dy);
}