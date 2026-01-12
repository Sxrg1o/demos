#ifndef ANT_WORLD_H
#define ANT_WORLD_H

#include "ant_math.h"
#include <stdbool.h>

typedef struct {
  int width;
  int height;
  bool **grid;
  int **food_scent;
} World;

int world_init(World *w, int width, int height);
void world_free(World *w);
bool world_in_bounds(const World *w, Position p);
bool world_is_occupied(const World *w, Position p);
void world_occupy_cell(World *w, Position p);
void world_vacate_cell(World *w, Position p);

#endif // ANT_WORLD_H
