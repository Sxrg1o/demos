#ifndef ANT_H
#define ANT_H

#include "../system/ant_math.h"
#include "resource.h"
#include <stdbool.h>

#define PLAN_SIZE 3
#define ANT_DEFAULT_LIFE 100
#define ANT_DEFAULT_WEIGHT 10
#define ANT_DEFAULT_RADIUS 5

typedef enum {
  ACTION_WAIT,
  ACTION_MOVE_LEFT,
  ACTION_MOVE_UP_LEFT,
  ACTION_MOVE_UP,
  ACTION_MOVE_UP_RIGHT,
  ACTION_MOVE_RIGHT,
  ACTION_MOVE_DOWN_RIGHT,
  ACTION_MOVE_DOWN,
  ACTION_MOVE_DOWN_LEFT,
  ACTION_PICKUP,
  ACTION_DROP
} ActionType;

typedef struct {
  int radius;
  int diameter;
  // [0][0] top-left corner,
  // [radius][radius] ant location (center).
  bool **obstacles;
  int **food_scent;
} LocalView;

typedef struct {
  Position position;
  int attention_radius;
  int velocity;
  int weight_g;
  int life;
  Resource *carried_resource;
  ActionType action_queue[PLAN_SIZE];
  int actions_queued;
} Ant;

// Lifecycle
void ant_init(Ant *ant, Position pos);
void ant_free(Ant *ant);

// Actions
void ant_think(Ant *ant, const LocalView *view);
bool ant_next_action(Ant *ant);
void ant_clear_plan(Ant *ant);
bool ant_move(Ant *ant, Position pos);
bool ant_carry(Ant *ant, Resource *resource);
void ant_consume(Ant *ant);
bool ant_drop(Ant *ant, Position pos);

Position action_to_offset(ActionType action);

#endif // ANT_H