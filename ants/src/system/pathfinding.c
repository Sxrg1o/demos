#include "pathfinding.h"
#include "ant_math.h"
#include <math.h>
#include <stdlib.h>

int pf_is_walkable(const World *w, Position p) {
  if (!w || !world_in_bounds(w, p))
    return 0;
  Cell c = w->grid[p.y * w->width + p.x];
  if (c.type == CELL_RESOURCE || c.type == CELL_NEST)
    return 0;
  return 1;
}

static float heuristic(Position a, Position b) {
  // Euclidean distance (admissible for grid with diagonal moves)
  return sqrtf(dist_sq(a, b));
}

int pf_astar(World *w, Position start, Position goal, Position *out_path,
                    int max_len) {
  if (!w || !out_path || max_len <= 0)
    return -1;
  if (!world_in_bounds(w, start) || !world_in_bounds(w, goal))
    return -1;

  if (Position_equals(start, goal)) {
    out_path[0] = start;
    return 1;
  }

  int width = w->width;
  int height = w->height;
  int n = width * height;

  float *g = malloc(sizeof(float) * n);
  float *f = malloc(sizeof(float) * n);
  int *came = malloc(sizeof(int) * n);
  unsigned char *open = calloc(n, 1);
  unsigned char *closed = calloc(n, 1);
  if (!g || !f || !came || !open || !closed) {
    free(g);
    free(f);
    free(came);
    free(open);
    free(closed);
    return -1;
  }

  for (int i = 0; i < n; i++) {
    g[i] = INFINITY;
    f[i] = INFINITY;
    came[i] = -1;
  }

  int start_i = start.y * width + start.x;
  int goal_i = goal.y * width + goal.x;

  g[start_i] = 0.0f;
  f[start_i] = heuristic(start, goal);
  open[start_i] = 1;

  while (1) {
    // find open node with lowest f
    int current = -1;
    float best_f = INFINITY;
    for (int i = 0; i < n; i++) {
      if (open[i] && !closed[i] && f[i] < best_f) {
        best_f = f[i];
        current = i;
      }
    }
    if (current == -1) {
      // no path
      break;
    }

    if (current == goal_i) {
      // reconstruct path
      int rev_count = 0;
      int node = current;
      // temporary storage for indices
      int *rev = malloc(sizeof(int) * n);
      if (!rev)
        break;
      while (node != -1 && rev_count < n) {
        rev[rev_count++] = node;
        node = came[node];
      }
      if (rev_count > max_len) {
        free(rev);
        break;
      }
      // reverse into out_path
      for (int i = 0; i < rev_count; i++) {
        int idx = rev[rev_count - 1 - i];
        int x = idx % width;
        int y = idx / width;
        out_path[i].x = x;
        out_path[i].y = y;
      }
      free(rev);
      free(g);
      free(f);
      free(came);
      free(open);
      free(closed);
      return rev_count;
    }

    open[current] = 0;
    closed[current] = 1;

    int cx = current % width;
    int cy = current / width;

    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        int nx = cx + dx;
        int ny = cy + dy;
        Position np = {nx, ny};
        if (!pf_is_walkable(w, np))
          continue;
        int ni = ny * width + nx;
        if (closed[ni])
          continue;

        float move_cost = (dx == 0 || dy == 0) ? 1.0f : 1.41421356f;
        float tentative_g = g[current] + move_cost;

        if (!open[ni] || tentative_g < g[ni]) {
          came[ni] = current;
          g[ni] = tentative_g;
          Position ppos = {nx, ny};
          f[ni] = g[ni] + heuristic(ppos, goal);
          open[ni] = 1;
        }
      }
    }
  }

  free(g);
  free(f);
  free(came);
  free(open);
  free(closed);
  return -1;
}
