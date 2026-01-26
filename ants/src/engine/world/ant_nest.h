#ifndef ANT_NEST_H
#define ANT_NEST_H

#include "ant.h"
#define MAX_ANTS 100

#define NEST_INITIAL_RADIUS 3
#define NEST_RADIUS_GROWTH_FACTOR 10

typedef struct {
  AntVector position;
  int radius;
  int stored_food;
  Ant ants[MAX_ANTS];
  int num_ants;
} AntNest;

void nest_init(AntNest *nest, AntVector position);
bool nest_pos_is_in_radius(AntNest *an, AntVector position);
void nest_update_radius(AntNest *nest);
void nest_set_food(AntNest *nest, int amount);
bool nest_add_ant(AntNest *nest, Ant ant);
Ant *nest_get_ant(AntNest *nest, int index);

#endif // ANT_NEST_H

