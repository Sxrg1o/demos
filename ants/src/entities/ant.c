#include "ant.h"
#include "resource.h"
#include <stdlib.h>
#include <string.h>

void ant_init(Ant *ant, Position pos) {
  if (!ant) {
    return;
  }
  memset(ant, 0, sizeof(*ant));
  ant->position = pos;
  ant->orientation = ORIENTATION_SOUTH;
  ant->carried_resource = NULL;
  ant->plan_length = 0;
}

void ant_free(Ant *ant) {
  if (!ant) {
    return;
  }
  memset(ant, 0, sizeof(*ant));
}

void ant_think(Ant *ant, const World *w) {
  // TODO: Implement AI decision making
}

bool ant_has_next_action(const Ant *ant) {
  if (!ant) {
    return false;
  }
  return ant->plan_idx < ant->plan_length;
}

bool ant_next_action(Ant *ant, Action *action) {
  if (!ant || !action) {
    return false;
  }
  *action = ant->plan[ant->plan_idx++];
  return true;
}

void ant_clear_plan(Ant *ant) {
  memset(ant->plan, 0, sizeof(Action) * ant->plan_length);
  ant->plan_idx = 0;
  ant->plan_length = 0;
}

Action action_create_idle() {
  return (Action){.resource = NULL, .target = {}, .type = ACTION_IDLE};
}

Action action_create_move(const Position dest) {
  return (Action){.resource = NULL, .target = dest, .type = ACTION_MOVE};
}

Action action_create_pickup(const Resource *resource) {
  Resource *r_copy = malloc(sizeof(Resource));
  memcpy(r_copy, resource, sizeof(Resource));
  return (Action){
      .resource = r_copy, .target = resource->position, .type = ACTION_PICKUP};
}

Action action_create_drop(const Resource *resource, const Position pos) {
  Resource *r_copy = malloc(sizeof(Resource));
  memcpy(r_copy, resource, sizeof(Resource));
  return (Action){.resource = r_copy, .target = pos, .type = ACTION_DROP};
}

void action_free(Action *action) {
  if (!action) {
    return;
  }

  if (action->resource) {
    resource_free(action->resource);
    action->resource = NULL;
  }
  memset(action, 0, sizeof(Action));
}
