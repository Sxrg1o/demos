#ifndef ANT_WORLD_H
#define ANT_WORLD_H

#include "../../entities/ant.h"
#include "../../entities/resource.h"
#include "../logic/ant_math.h"
#include <stdbool.h>
#define MAX_ANTS 100

#define EVAPORATION_RATE 0.8f
#define DIFFUSION_RATE 0.2f
#define NEST_INITIAL_RADIUS 3

typedef struct {
  Position position;
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
  float pheromone_to_food; // scent to food (red)
  float pheromone_to_home; // scent to home (blue)
  float pheromone_visited; // scent for exploration
  Resource resource;       // if type is CELL_RESOURCE
  int ant_id;              // if type is CELL_ANT (-1 = none)
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
bool world_in_bounds(const World *w, Position p);
bool world_is_occupied(const World *w, Position p);
void world_occupy_cell(World *w, Position p, Cell cell);
void world_vacate_cell(World *w, Position p);

void nest_update_radius(AntNest *nest);
void nest_set_food(AntNest *nest, int amount);
#endif // ANT_WORLD_H
