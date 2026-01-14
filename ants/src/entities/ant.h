#ifndef ANT_H
#define ANT_H

#include "../system/ant_math.h"
#include "resource.h"
#include <stdbool.h>

#define PLAN_SIZE 8
#define ANT_RADIUS 16

typedef enum {
  STATE_SCOUTING,   // Exploring (Looking for food)
  STATE_HARVESTING, // Found trail, going to food
  STATE_RETURNING,  // Carrying food, going to nest
  STATE_CLEARING    // Carrying dirt, clearing path
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
  
  // Intelligence
  AntState state;
  Position origin_pos; // Where I picked up the current burden
  int frustration;
} Ant;

// Lifecycle
void ant_init(Ant *ant, Position pos, Position nest_pos);
void ant_free(Ant *ant);

// Plan
bool ant_next_action(Ant *ant, Action *out_action);
void ant_clear_plan(Ant *ant);

#endif // ANT_H
