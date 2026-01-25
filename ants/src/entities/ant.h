#ifndef ANT_H
#define ANT_H

#include "../engine/logic/ant_math.h"
#include "resource.h"
#include <stdbool.h>

#define PLAN_SIZE 10
#define ANT_RADIUS 10

typedef enum {
  STATE_SCOUTING,      // Exploring (Looking for food)
  STATE_HARVESTING,    // Found trail, going to food
  STATE_RETURNING,     // Carrying food, going to nest
  STATE_CLEARING,      // Carrying dirt, clearing path
  STATE_SEARCHING_HOME // Lost ant with food
} AntState;

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
  bool is_carring;
  Resource carried_resource;
  Action plan[PLAN_SIZE];
  int plan_length;
  int plan_idx;
  Position nest_position;
  int id;

  // Intelligence
  AntState state;
  Position origin_pos; // Where I picked up the current burden
  int frustration;
} Ant;

// Lifecycle
void ant_init(Ant *ant, int ant_id, Position pos, Position nest_pos);
void ant_free(Ant *ant);

// Plan
bool ant_next_action(Ant *ant, Action *out_action);
void ant_clear_plan(Ant *ant);

#endif // ANT_H
