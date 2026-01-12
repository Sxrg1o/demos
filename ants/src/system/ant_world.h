#ifndef ANT_WORLD_H
#define ANT_WORLD_H

#include "../entities/resource.h"
#include "ant_math.h"
#include <stdbool.h>

typedef enum {
  CELL_EMPTY,
  CELL_RESOURCE,
  CELL_ANT,
} CellType;

typedef struct {
  float pheromone_home;
  float pheromone_food;
  Resource resource; // if type is CELL_RESOURCE
  int ant_id;        // if type is CELL_ANT
  CellType type;
} Cell;

typedef struct {
  int width;
  int height;
  Cell *grid;
} World;

int world_init(World *w, int width, int height);
void world_free(World *w);
bool world_in_bounds(const World *w, Position p);
bool world_is_occupied(const World *w, Position p);
void world_occupy_cell(World *w, Position p, Cell cell);
void world_vacate_cell(World *w, Position p);

#endif // ANT_WORLD_H
