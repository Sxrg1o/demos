#ifndef ANT_H
#define ANT_H

#include "../system/ant_math.h"
#include "resource.h"
#include <stdbool.h>

#define PLAN_SIZE 3
#define ANT_DEFAULT_LIFE 100
#define ANT_DEFAULT_RADIUS 5

typedef enum {
  ORIENTATION_NORTH,
  ORIENTATION_NORTH_EAST,
  ORIENTATION_EAST,
  ORIENTATION_SOUTH_EAST,
  ORIENTATION_SOUTH,
  ORIENTATION_SOUTH_WEST,
  ORIENTATION_WEST,
  ORIENTATION_NORTH_WEST
} Orientation;

typedef enum {
  ACTION_IDLE,
  ACTION_MOVE_TO,
  ACTION_PICKUP,
  ACTION_DROP
} ActionType;

typedef struct {
  ActionType type;
  Position target;
  Resource *resource;
} Action;

typedef struct {
  int radius;
  int diameter;
  bool **obstacles;
  int **food_scent;
} LocalView;

typedef struct {
  Position position;
  Orientation orientation;
  int life;
  Resource *carried_resource;
  Action current_plan[PLAN_SIZE];
  int plan_length;
} Ant;

// Lifecycle
void ant_init(Ant *ant, Position pos);
void ant_free(Ant *ant);

// Actions
void ant_think(Ant *ant, const LocalView *view);
bool ant_get_current_action(const Ant *ant, Action *action);
void ant_advance_plan(Ant *ant);
void ant_clear_plan(Ant *ant);

Position orientation_to_offset(Orientation orient);
Orientation offset_to_orientation(Position offset);

#endif // ANT_H
