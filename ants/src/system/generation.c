#include "generation.h"
#include "ant_math.h"
#include "ant_world.h"
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

      c->pheromone_to_food = 0.0f;
      c->pheromone_to_home = 0.0f;
      c->pheromone_visited = 0.0f;
      c->ant_id = -1;
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
  world->nest.position = (Position){.x = nest_x, .y = nest_y};
  world->grid[nest_y * world->width + nest_x].type = CELL_NEST;
}
