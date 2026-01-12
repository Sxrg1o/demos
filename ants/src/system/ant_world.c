#include "ant_world.h"
#include <stdlib.h>
#include <string.h>

int world_init(World *w, int width, int height) {
  if (!w || width <= 0 || height <= 0) {
    return -1;
  }

  w->width = width;
  w->height = height;

  w->grid = malloc(sizeof(bool *) * height);
  if (!w->grid) {
    return -1;
  }

  for (int i = 0; i < height; i++) {
    w->grid[i] = calloc(width, sizeof(bool));
    if (!w->grid[i]) {
      for (int j = 0; j < i; j++) {
        free(w->grid[j]);
      }
      free(w->grid);
      w->grid = NULL;
      return -1;
    }
  }

  return 0;
}

void world_free(World *w) {
  if (!w || !w->grid) {
    return;
  }

  for (int i = 0; i < w->height; i++) {
    free(w->grid[i]);
  }
  free(w->grid);

  w->grid = NULL;
  w->width = 0;
  w->height = 0;
}

bool world_in_bounds(const World *w, Position p) {
  if (!w) {
    return false;
  }
  return p.x >= 0 && p.x < w->width && p.y >= 0 && p.y < w->height;
}

bool world_is_occupied(const World *w, Position p) {
  if (!w || !w->grid || !world_in_bounds(w, p)) {
    return true;
  }
  return w->grid[p.y][p.x];
}

void world_occupy_cell(World *w, Position p) {
  if (!w || !w->grid || !world_in_bounds(w, p)) {
    return;
  }
  w->grid[p.y][p.x] = true;
}

void world_vacate_cell(World *w, Position p) {
  if (!w || !w->grid || !world_in_bounds(w, p)) {
    return;
  }
  w->grid[p.y][p.x] = false;
}
