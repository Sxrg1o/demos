#include "ant.h"
#include "resource.h"
#include <string.h>

static const Position DIRECTION_OFFSETS[] = {
    [ACTION_WAIT] = {0, 0},
    [ACTION_MOVE_LEFT] = {-1, 0},
    [ACTION_MOVE_UP_LEFT] = {-1, -1},
    [ACTION_MOVE_UP] = {0, -1},
    [ACTION_MOVE_UP_RIGHT] = {1, -1},
    [ACTION_MOVE_RIGHT] = {1, 0},
    [ACTION_MOVE_DOWN_RIGHT] = {1, 1},
    [ACTION_MOVE_DOWN] = {0, 1},
    [ACTION_MOVE_DOWN_LEFT] = {-1, 1},
    [ACTION_PICKUP] = {0, 0},
    [ACTION_DROP] = {0, 0},
};

Position action_to_offset(ActionType action) {
  if (action < 0 || action > ACTION_DROP) {
    return (Position){0, 0};
  }
  return DIRECTION_OFFSETS[action];
}

void ant_init(Ant *ant, Position pos) {
  if (!ant) {
    return;
  }

  memset(ant, 0, sizeof(*ant));
  ant->position = pos;
  ant->attention_radius = ANT_DEFAULT_RADIUS;
  ant->velocity = 1;
  ant->weight_g = ANT_DEFAULT_WEIGHT;
  ant->life = ANT_DEFAULT_LIFE;
  ant->carried_resource = NULL;
  ant->actions_queued = 0;
}

void ant_free(Ant *ant) {
  if (!ant) {
    return;
  }
  memset(ant, 0, sizeof(*ant));
}

void ant_think(Ant *ant, const LocalView *view) {
  if (!ant || !view) {
    return;
  }
}

bool ant_next_action(Ant *ant) {
  if (!ant || ant->actions_queued <= 0) {
    return false;
  }

  for (int i = 0; i < ant->actions_queued - 1; i++) {
    ant->action_queue[i] = ant->action_queue[i + 1];
  }
  ant->actions_queued--;

  return true;
}

void ant_clear_plan(Ant *ant) {
  if (!ant) {
    return;
  }
  ant->actions_queued = 0;
}

bool ant_move(Ant *ant, Position pos) {
  if (!ant) {
    return false;
  }
  ant->position = pos;
  return true;
}

bool ant_carry(Ant *ant, Resource *resource) {
  if (!ant || !resource) {
    return false;
  }

  ant->carried_resource = resource;
  return true;
}

void ant_consume(Ant *ant) {
  if (!ant || !ant->carried_resource) {
    return;
  }
}

bool ant_drop(Ant *ant, Position pos) {
  if (!ant || !ant->carried_resource) {
    return false;
  }

  ant->carried_resource->position = pos;
  ant->carried_resource = NULL;
  return true;
}
