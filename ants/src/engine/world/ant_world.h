#ifndef ANT_WORLD_H
#define ANT_WORLD_H

#include "ant.h"
#include "resource.h"
#include "../logic/ant_math.h"
#include <stdbool.h>
#define MAX_ANTS 100

#define EVAPORATION_RATE 0.8f
#define DIFFUSION_RATE 0.2f
#define NEST_INITIAL_RADIUS 3
#define NEST_RADIUS_GROWTH_FACTOR 10

typedef struct {
  AntVector position;
  int radius;
  int stored_food;
  Ant ants[MAX_ANTS];
  int num_ants;
} AntNest;

typedef enum {
  CELL_EMPTY,
  CELL_RESOURCE,
  CELL_ANT,
  CELL_NEST,
  CELL_OUT_OF_BOUNDS,
} CellType;

typedef struct {
  float pheromone_food;  // scent to food
  float pheromone_build; // scent for building
  Resource resource;     // if type is CELL_RESOURCE
  uint8_t bg_variant;    // 0-255 for visual noise (background/empty)
  CellType type;
} Cell;

typedef struct {
  int width;
  int height;
  Cell *grid;
  AntNest nest;
} World;

int world_init(World *w, int width, int height);
void world_free(World *w);
bool world_in_bounds(const World *w, AntVector p);
bool world_is_occupied(const World *w, AntVector p);
void world_occupy_cell(World *w, AntVector p, Cell cell);
void world_vacate_cell(World *w, AntVector p);
Cell world_get_cell(World *w, AntVector p);

void nest_update_radius(AntNest *nest);
void nest_set_food(AntNest *nest, int amount);
#endif // ANT_WORLD_H
