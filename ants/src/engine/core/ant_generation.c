#include "ant_generation.h"
#include <stdlib.h>

void gen_world(World *world, unsigned int seed) {
  srand(seed);
  float surface_base = world->height * 0.3f;
  float surface_amp = world->height * 0.1f;
  float surface_freq = 0.1f;
  float cave_freq = 0.15f;
  float cave_threshold = 0.6f;

  float factor = 0.75 + (rand() % 50 + 1) / 100.0; // between 0.75 and 1.25
  int num_deposits = 1e-3 * world->height * world->width * factor;

  for (int x = 0; x < world->width; x++) {
    float n_surf = noise2d(x * surface_freq, 0.0f, seed);
    int surface_y = (int)(surface_base + (n_surf * 2.0f - 1.0f) * surface_amp);

    for (int y = 0; y < world->height; y++) {
      Cell *c = &world->grid[y * world->width + x];

      c->pheromone_food = 0.0f;
      c->pheromone_build = 0.0f;
      c->type = CELL_EMPTY;

      if (y >= surface_y) {
        c->type = CELL_RESOURCE;
        c->resource = resource_dirt();
        float n_cave = noise2d(x * cave_freq, y * cave_freq + 100.0f, seed);
        if (n_cave > cave_threshold) {
          c->type = CELL_EMPTY;
        }
      }
    }
  }

  for (int i = 0; i < num_deposits; i++) {
    int cx = rand() % world->width;
    int min_y = (int)(surface_base + surface_amp) + 5;
    if (min_y >= world->height)
      min_y = world->height - 1;

    int cy = min_y + (rand() % (world->height - min_y));

    int radius = 1 + (rand() % 4);

    for (int dy = -radius; dy <= radius; dy++) {
      for (int dx = -radius; dx <= radius; dx++) {
        if (dx * dx + dy * dy <= radius * radius) {
          int px = cx + dx;
          int py = cy + dy;

          if (px >= 0 && px < world->width && py >= 0 && py < world->height) {
            Cell *c = &world->grid[py * world->width + px];
            if (c->type == CELL_EMPTY || (c->type == CELL_RESOURCE &&
                                          c->resource.type == RESOURCE_DIRT)) {
              c->type = CELL_RESOURCE;
              c->resource = resource_food();
            }
          }
        }
      }
    }
  }

  int nest_margin = 10;
  int nest_radius = 6;
  int nest_x = nest_margin + rand() % (world->width - 2 * nest_margin);
  int nest_y_min = (int)(world->height * 0.5f);
  int nest_y = nest_y_min + rand() % (world->height - nest_margin - nest_y_min);

  for (int dy = -nest_radius; dy <= 0; dy++) {
    for (int dx = -nest_radius; dx <= nest_radius; dx++) {
      if (dx * dx + dy * dy <= nest_radius * nest_radius) {
        int px = nest_x + dx;
        int py = nest_y + dy;
        if (px >= 0 && px < world->width && py >= 0 && py < world->height) {
          world->grid[py * world->width + px].type = CELL_EMPTY;
        }
      } else if (dx * dx + dy * dy <= (nest_radius + 2) * (nest_radius + 2)) {
        int px = nest_x + dx;
        int py = nest_y + dy;
        if (px >= 0 && px < world->width && py >= 0 && py < world->height) {
          world->grid[py * world->width + px].type = CELL_RESOURCE;
          world->grid[py * world->width + px].resource = resource_dirt();
        }
      }
    }
  }

  int dy = 1;
  for (int dx = -nest_radius; dx <= nest_radius; dx++) {
    int px = nest_x + dx;
    int py = nest_y + dy;
    if (px >= 0 && px < world->width && py >= 0 && py < world->height) {
      world->grid[py * world->width + px].type = CELL_RESOURCE;
      world->grid[py * world->width + px].resource = resource_dirt();
    }
  }
  world->nest.position = (AntVector){.x = nest_x, .y = nest_y};
  world->grid[nest_y * world->width + nest_x].type = CELL_NEST;
}

void gen_flat_world(World *world, int empty_width) {
  for (int x = 0; x < world->width; x++) {
    for (int y = 0; y < world->height; y++) {
      Cell *c = &world->grid[y * world->width + x];

      c->pheromone_food = 0.0f;
      c->pheromone_build = 0.0f;
      c->type = CELL_EMPTY;
    }
  }

  /*
    x x x x x x x x x x -> w->height / 2 - empty_radius
          ANT_NEST      -> empty_radius
    x x x x x x x x x x ->
  */
  int upper_bound = world->height / 2 - empty_width;
  int lower_bound = upper_bound + empty_width + 1;

  for (int y = 0; y < world->height; y++) {
    if (y < lower_bound && upper_bound < y) {
      continue;
    }
    for (int x = 0; x < world->width; x++) {
      Cell *c = &world->grid[y * world->width + x];
      c->resource = resource_dirt();
      c->type = CELL_RESOURCE;
    }
  }

  AntVector nest_pos = {.y = world->height / 2.0f, .x = world->width / 2.0f};
  world->nest.position = nest_pos;
  Cell *nest_cell =
      &world->grid[(int)nest_pos.y * world->width + (int)nest_pos.x];
  nest_cell->type = CELL_NEST;

  int food_radius = 4;
  AntVector left_fr = {.x = food_radius + 1, .y = world->height / 2.0f};
  AntVector right_fr = {.x = (world->width - 1) - food_radius - 1,
                        .y = world->height / 2.0};

  for (int dy = -food_radius; dy <= food_radius; dy++) {
    for (int dx = -food_radius; dx <= food_radius; dx++) {
      if ((dx * dx + dy * dy) > food_radius * food_radius) {
        continue;
      }
      int l_x = left_fr.x + dx;
      int l_y = left_fr.y + dy;
      int r_x = right_fr.x + dx;
      int r_y = right_fr.y + dy;
      if (!world_in_bounds(world, (AntVector){.x = l_x, .y = l_y})) {
        continue;
      }
      if (!world_in_bounds(world, (AntVector){.x = r_x, .y = r_y})) {
        continue;
      }

      Cell *l_c = &world->grid[l_y * world->width + l_x];
      Cell *r_c = &world->grid[r_y * world->width + r_x];

      l_c->resource = resource_food();
      l_c->type = CELL_RESOURCE;

      r_c->resource = resource_food();
      r_c->type = CELL_RESOURCE;
    }
  }
}
