#include "ant.h"
#include <string.h>

void ant_init(Ant *ant, int ant_id, Position pos, Position nest_pos) {
  if (!ant)
    return;

  ant->position = pos;
  ant->nest_position = nest_pos;
  ant->id = ant_id;
  ant->is_carring = false;
  ant->carried_resource = (Resource){0};
  ant->plan_length = 0;
  ant->plan_idx = 0;
  memset(ant->plan, 0, sizeof(ant->plan));
  
  ant->state = STATE_SCOUTING;
  ant->origin_pos = (Position){0, 0};
  ant->frustration = 0;
}

void ant_free(Ant *ant) {
  // Nothing to free yet as there are no dynamic allocations inside Ant
}

bool ant_next_action(Ant *ant, Action *out_action) {
  if(!ant || !out_action) {
    return false;
  }
  if(ant->plan_idx < 0 || ant->plan_idx >= ant->plan_length) {
    return false;
  }
  *out_action = ant->plan[ant->plan_idx++];
  return true;
}

void ant_clear_plan(Ant *ant) {
  if(!ant) {
    return;
  }
  memset(ant->plan, 0, sizeof(Action) * PLAN_SIZE);
  ant->plan_idx = 0;
  ant->plan_length = 0;
}

