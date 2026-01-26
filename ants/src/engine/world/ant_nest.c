#include "ant_nest.h"
#include <stddef.h>

void nest_init(AntNest *nest, AntVector position) {
  if (!nest) {
    return;
  }
  nest->position = position;
  nest->radius = NEST_INITIAL_RADIUS;
  nest->stored_food = 0;
  nest->num_ants = 0;
}

bool nest_pos_is_in_radius(AntNest *an, AntVector position) {
  if (!an) {
    return false;
  }
  float dist = dist_sq(an->position, position);
  return dist <= (an->radius * an->radius);
}

void nest_update_radius(AntNest *nest) {
  if (!nest) {
    return;
  }
  nest->radius =
      NEST_INITIAL_RADIUS + (nest->stored_food / NEST_RADIUS_GROWTH_FACTOR);
}

void nest_set_food(AntNest *nest, int amount) {
  if (!nest) {
    return;
  }
  nest->stored_food = amount;
  if (nest->stored_food < 0) {
    nest->stored_food = 0;
  }
  nest_update_radius(nest);
}

bool nest_add_ant(AntNest *nest, Ant ant) {
  if (!nest || nest->num_ants >= MAX_ANTS) {
    return false;
  }
  nest->ants[nest->num_ants++] = ant;
  return true;
}

Ant *nest_get_ant(AntNest *nest, int index) {
  if (!nest || index < 0 || index >= nest->num_ants) {
    return NULL;
  }
  return &nest->ants[index];
}
