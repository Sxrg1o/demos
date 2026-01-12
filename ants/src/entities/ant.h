#ifndef ANT_H
#define ANT_H

#include "../system/ant_math.h"
#include "../system/ant_world.h"
#include "resource.h"
#include <stdbool.h>

#define PLAN_SIZE 3
#define ANT_DEFAULT_LIFE 100
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
  Resource *resource;
} Action;

typedef struct {
  Position position;
  Orientation orientation;
  Resource *carried_resource;
  Action plan[PLAN_SIZE];
  int plan_length;
  int plan_idx;
} Ant;

// Lifecycle
void ant_init(Ant *ant, Position pos);
void ant_free(Ant *ant);

// Plan
void ant_think(Ant *ant, const World *w);
bool ant_next_action(Ant *ant, Action *action);
void ant_clear_plan(Ant *ant);

// Action
Action action_create_idle();
Action action_create_move(const Position dest);
Action action_create_pickup(const Resource *resource);
Action action_create_drop(const Resource *resource, const Position pos);
void action_free(Action *action);

#endif // ANT_H
