#ifndef ANT_H
#define ANT_H

#include "../system/ant_math.h"
#include "resource.h"
#include <stdbool.h>

#define PLAN_SIZE 3
#define ANT_DEFAULT_RADIUS 5

typedef enum {
  ORIENTATION_NORTH,
  ORIENTATION_EAST,
  ORIENTATION_SOUTH,
  ORIENTATION_WEST,
} Orientation;

typedef enum {
  ACTION_IDLE,
  ACTION_MOVE,
  ACTION_PICKUP,
  ACTION_DROP
} ActionType;

typedef struct {
  ActionType type;
  Position target;
  Resource resource;
} Action;

typedef struct {
  Position position;
  Orientation orientation;
  bool is_carring;
  Resource carried_resource;
  Action plan[PLAN_SIZE];
  int plan_length;
  int plan_idx;
} Ant;

// Lifecycle
void ant_init(Ant *ant, Position pos);
void ant_free(Ant *ant);

#endif // ANT_H
