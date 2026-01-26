#include "simulation.h"
#include <stdlib.h>

// Entities Management
bool entity_spawn_ant(World *w, AntVector p) {
  if (!w || !world_in_bounds(w, p)) {
    return false;
  }

  if (w->nest.num_ants >= MAX_ANTS) {
    return false;
  }

  if (world_is_occupied(w, p)) {
    return false;
  }

  int id = w->nest.num_ants;
  Ant *ant = &w->nest.ants[id];
  ant_init(ant, id, p, w->nest.position);
  Cell cell = w->grid[(int)p.y * w->width + (int)p.x];
  cell.type = CELL_ANT;

  world_occupy_cell(w, p, cell);

  w->nest.num_ants++;
  return true;
}

bool entity_spawn_food(World *w, AntVector p, int radius) {
  if (!w)
    return false;

  bool placed = false;
  int r2 = radius * radius;

  for (int dy = -radius; dy <= radius; dy++) {
    for (int dx = -radius; dx <= radius; dx++) {
      if (dx * dx + dy * dy <= r2) {
        AntVector t = {p.x + dx, p.y + dy};

        if (world_in_bounds(w, t)) {
          Cell *c = &w->grid[(int)t.y * w->width + (int)t.x];

          c->type = CELL_RESOURCE;
          c->resource.type = RESOURCE_FOOD;
          c->resource.value = 10;
          c->resource.weight = 1;
          placed = true;
        }
      }
    }
  }
  return placed;
}

void system_update_world(World *w) {
  if (!w) {
    return;
  }

  /* Pheromone diffusion + evaporation + source injection */
  int width = w->width;
  int height = w->height;
  int n = width * height;

  float *new_food = calloc(n, sizeof(float));
  float *new_home = calloc(n, sizeof(float));
  float *new_visited = calloc(n, sizeof(float));
  if (!new_food || !new_home || !new_visited) {
    free(new_food);
    free(new_home);
    free(new_visited);
    return;
  }

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int idx = y * width + x;
      Cell *c = &w->grid[idx];

      int neigh[8];
      int nc = 0;
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0)
            continue;
          int nx = x + dx;
          int ny = y + dy;
          if (nx < 0 || nx >= width || ny < 0 || ny >= height)
            continue;
          neigh[nc++] = ny * width + nx;
        }
      }

      float v_food = c->pheromone_food;
      float v_home = c->pheromone_build;

      float d_food = v_food * DIFFUSION_RATE;
      float d_home = v_home * DIFFUSION_RATE;

      float keep_food = v_food - d_food;
      float keep_home = v_home - d_home;

      new_food[idx] += keep_food;
      new_home[idx] += keep_home;

      if (nc > 0) {
        float share_food = d_food / nc;
        float share_home = d_home / nc;
        for (int k = 0; k < nc; k++) {
          int ni = neigh[k];
          new_food[ni] += share_food;
          new_home[ni] += share_home;
        }
      } else {
        new_food[idx] += d_food;
        new_home[idx] += d_home;
      }
    }
  }

  /* evaporation */
  for (int i = 0; i < n; i++) {
    new_food[i] *= EVAPORATION_RATE;
    new_home[i] *= EVAPORATION_RATE;
    new_visited[i] *= EVAPORATION_RATE;
  }

  for (int i = 0; i < n; i++) {
    w->grid[i].pheromone_food = new_food[i];
    w->grid[i].pheromone_build = new_home[i];
  }

  if (world_in_bounds(w, w->nest.position)) {
    int ni = w->nest.position.y * width + w->nest.position.x;
    w->grid[ni].pheromone_build = 1.0f;
  }
  for (int i = 0; i < n; i++) {
    if (w->grid[i].type == CELL_RESOURCE &&
        w->grid[i].resource.type == RESOURCE_FOOD) {
      w->grid[i].pheromone_food = 1.0f;
    }
  }

  free(new_food);
  free(new_home);
  free(new_visited);

  // check nest radius update
  int nest_radius_sq = w->nest.radius * w->nest.radius;
  int food_within_radius = 0;
  for (int y = -w->nest.radius; y <= w->nest.radius; y++) {
    for (int x = -w->nest.radius; x <= w->nest.radius; x++) {
      if (x * x + y * y <= nest_radius_sq) {
        AntVector p = {w->nest.position.x + x, w->nest.position.y + y};
        if (world_in_bounds(w, p)) {
          Cell *c = &w->grid[(int)p.y * w->width + (int)p.x];
          if (c->type == CELL_RESOURCE && c->resource.type == RESOURCE_FOOD) {
            food_within_radius++;
          }
        }
      }
    }
  }

  nest_set_food(&w->nest, food_within_radius);
  nest_update_radius(&w->nest);
}