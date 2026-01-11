#include "generation.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
  float x, y;
} Vector2f;

static float dot_grid_gradient(int ix, int iy, float x, float y) {
  unsigned int w = 8 * sizeof(unsigned int);
  unsigned int s = w / 2;
  unsigned int a = ix, b = iy;
  a *= 3284157443;
  b ^= a << s | a >> (w - s);
  b *= 1911520717;
  a ^= b << s | b >> (w - s);
  a *= 2048419325;
  float random = a * (3.14159265f / ~(~0u >> 1)); // in [0, 2*Pi]

  Vector2f v;
  v.x = cosf(random);
  v.y = sinf(random);

  float dx = x - (float)ix;
  float dy = y - (float)iy;

  return (dx * v.x + dy * v.y);
}

static float interpolate(float a0, float a1, float w) {
  return (a1 - a0) * (3.0f - w * 2.0f) * w * w + a0;
}

static float perlin(float x, float y) {
  int x0 = (int)floorf(x);
  int y0 = (int)floorf(y);
  int x1 = x0 + 1;
  int y1 = y0 + 1;

  float sx = x - (float)x0;
  float sy = y - (float)y0;

  float n0, n1, ix0, ix1, value;

  n0 = dot_grid_gradient(x0, y0, x, y);
  n1 = dot_grid_gradient(x1, y0, x, y);
  ix0 = interpolate(n0, n1, sx);

  n0 = dot_grid_gradient(x0, y1, x, y);
  n1 = dot_grid_gradient(x1, y1, x, y);
  ix1 = interpolate(n0, n1, sx);

  value = interpolate(ix0, ix1, sy);
  return value; // Range roughly -1.0 to 1.0
}

static float fbm(float x, float y, int octaves) {
  float val = 0.0f;
  float amp = 1.0f;
  float freq = 1.0f;
  float max = 0.0f;
  for (int i = 0; i < octaves; i++) {
    val += perlin(x * freq, y * freq) * amp;
    max += amp;
    amp *= 0.5f;
    freq *= 2.0f;
  }
  return val / max;
}

int generate_world_content(World *world, Resource *resources,
                           int max_resources) {
  if (!world)
    return 0;

  float zoom = 0.05f;
  float surface_zoom = 0.03f;
  int surface_base = (int)(world->height * 0.25f);

  for (int x = 0; x < world->width; x++) {
    float n_surf = perlin(x * surface_zoom, 0);
    int surface_level = surface_base + (int)(n_surf * 15.0f);

    for (int y = 0; y < world->height; y++) {
      bool is_solid = false;

      if (y > surface_level) {
        float n_cave = fbm(x * zoom, y * zoom, 3);
        if (n_cave < 0.35f) {
          is_solid = true;
        }
      }

      if (is_solid) {
        world_occupy_cell(world, (Position){x, y});
      } else {
        world_vacate_cell(world, (Position){x, y});
      }
    }
  }

  int count = 0;
  int tries = 0;
  int max_tries = 1000;

  while (count < max_resources && tries < max_tries) {
    tries++;

    int cx = rand() % world->width;
    int cy = rand() % world->height;

    if (!world_is_occupied(world, (Position){cx, cy}))
      continue;

    if (cy < surface_base + 20)
      continue;

    int cluster_radius = 2 + (rand() % 3); // 2 to 4

    for (int dy = -cluster_radius; dy <= cluster_radius; dy++) {
      for (int dx = -cluster_radius; dx <= cluster_radius; dx++) {
        if (count >= max_resources)
          break;

        if (dx * dx + dy * dy <= cluster_radius * cluster_radius) {
          Position p = {cx + dx, cy + dy};
          if (world_is_occupied(world, p)) {
            resource_init(&resources[count], p, 10, 1, RESOURCE_FOOD);
            count++;
          }
        }
      }
    }
  }

  return count;
}
