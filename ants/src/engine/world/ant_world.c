#include "ant_world.h"
#include <stdlib.h>
#include <string.h>

int world_init(World *w, int width, int height) {
  if (!w || width <= 0 || height <= 0) {
    return -1;
  }

  w->width = width;
  w->height = height;
  memset(&w->nest.ants, 0, sizeof(Ant) * MAX_ANTS);
  w->nest.num_ants = 0;
  w->nest.stored_food = 0;
  w->nest.radius = NEST_INITIAL_RADIUS;
  w->nest.position.x = 0;
  w->nest.position.y = 0;
  w->grid = malloc(sizeof(Cell) * width * height);
  if (!w->grid) {
    return -1;
  }
  memset(w->grid, 0, sizeof(Cell) * width * height);
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      Cell *c = &w->grid[y * width + x];
      c->type = CELL_EMPTY;
      c->ant_id = -1;
      c->pheromone_to_food = 0.0f;
      c->pheromone_to_home = 0.0f;
      c->pheromone_visited = 0.0f;
    }
  }
  return 0;
}

void world_free(World *w) {
  if (!w || !w->grid) {
    return;
  }

  free(w->grid);
  w->grid = NULL;
  w->width = 0;
  w->height = 0;
  memset(&w->nest.ants, 0, sizeof(Ant) * MAX_ANTS);
  w->nest.num_ants = 0;
  w->nest.stored_food = 0;
  w->nest.radius = 0;
  w->nest.position.x = 0;
  w->nest.position.y = 0;
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
  return w->grid[p.y * w->width + p.x].type != CELL_EMPTY;
}

void world_occupy_cell(World *w, Position p, Cell cell) {
  if (!w || !w->grid || !world_in_bounds(w, p)) {
    return;
  }
  Cell *dest = &w->grid[p.y * w->width + p.x];
  dest->resource = cell.resource;
  dest->ant_id = cell.ant_id;
  dest->type = cell.type;
  dest->pheromone_to_food = cell.pheromone_to_food;
  dest->pheromone_to_home = cell.pheromone_to_home;
  dest->pheromone_visited = cell.pheromone_visited;
}

void world_vacate_cell(World *w, Position p) {
  if (!w || !w->grid || !world_in_bounds(w, p)) {
    return;
  }
  Cell *c = &w->grid[p.y * w->width + p.x];
  c->type = CELL_EMPTY;
  c->ant_id = -1;
}

void nest_update_radius(AntNest *nest) {
  if (!nest) {
    return;
  }
  // updates radius based on stored food amount
  nest->radius = NEST_INITIAL_RADIUS + (nest->stored_food / 10);
}

void nest_set_food(AntNest *nest, int amount) {
  if (!nest) {
    return;
  }
  nest->stored_food = amount;
}