#ifndef ANT_WORLD_H
#define ANT_WORLD_H

#include "../entities/ant.h"
#include "../entities/resource.h"
#include "ant_math.h"
#include <stdbool.h>
#define MAX_ANTS 100

typedef enum {
  CELL_EMPTY,
  CELL_RESOURCE,
  CELL_ANT,
  CELL_NEST,
  CELL_OUT_OF_BOUNDS,
} CellType;

typedef struct {
  float pheromone_food;
  Resource resource; // if type is CELL_RESOURCE
  int ant_id;        // if type is CELL_ANT
  CellType type;
} Cell;

typedef struct {
  int width;
  int height;
  Cell *grid;
  Ant ants[MAX_ANTS];
  int num_ants;
  Position nest_pos;
  int next_ant_idx;
} World;

int world_init(World *w, int width, int height);
void world_free(World *w);
bool world_in_bounds(const World *w, Position p);
bool world_is_occupied(const World *w, Position p);
void world_occupy_cell(World *w, Position p, Cell cell);
void world_vacate_cell(World *w, Position p);

#endif // ANT_WORLD_H
