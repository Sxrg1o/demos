#include "ant.h"
#include <string.h>

static const Position ORIENTATION_OFFSETS[] = {
    [ORIENTATION_NORTH] = {0, -1}, [ORIENTATION_NORTH_EAST] = {1, -1},
    [ORIENTATION_EAST] = {1, 0},   [ORIENTATION_SOUTH_EAST] = {1, 1},
    [ORIENTATION_SOUTH] = {0, 1},  [ORIENTATION_SOUTH_WEST] = {-1, 1},
    [ORIENTATION_WEST] = {-1, 0},  [ORIENTATION_NORTH_WEST] = {-1, -1},
};

Position orientation_to_offset(Orientation orient) {
  if (orient < 0 || orient > ORIENTATION_NORTH_WEST) {
    return (Position){0, 0};
  }
  return ORIENTATION_OFFSETS[orient];
}

Orientation offset_to_orientation(Position offset) {
  if (offset.x == 0 && offset.y == -1)
    return ORIENTATION_NORTH;
  if (offset.x == 1 && offset.y == -1)
    return ORIENTATION_NORTH_EAST;
  if (offset.x == 1 && offset.y == 0)
    return ORIENTATION_EAST;
  if (offset.x == 1 && offset.y == 1)
    return ORIENTATION_SOUTH_EAST;
  if (offset.x == 0 && offset.y == 1)
    return ORIENTATION_SOUTH;
  if (offset.x == -1 && offset.y == 1)
    return ORIENTATION_SOUTH_WEST;
  if (offset.x == -1 && offset.y == 0)
    return ORIENTATION_WEST;
  if (offset.x == -1 && offset.y == -1)
    return ORIENTATION_NORTH_WEST;
  return ORIENTATION_NORTH;
}

void ant_init(Ant *ant, Position pos) {
  if (!ant) {
    return;
  }
  memset(ant, 0, sizeof(*ant));
  ant->position = pos;
  ant->orientation = ORIENTATION_SOUTH;
  ant->life = ANT_DEFAULT_LIFE;
  ant->carried_resource = NULL;
  ant->plan_length = 0;
}

void ant_free(Ant *ant) {
  if (!ant) {
    return;
  }
  memset(ant, 0, sizeof(*ant));
}

void ant_think(Ant *ant, const LocalView *view) {
  // TODO: Implement AI decision making
}

bool ant_get_current_action(const Ant *ant, Action *action) {
  if (!ant || ant->plan_length <= 0 || !action) {
    return false;
  }
  *action = ant->current_plan[0];
  return true;
}

void ant_advance_plan(Ant *ant) {
  if (!ant || ant->plan_length <= 0) {
    return;
  }
  for (int i = 0; i < ant->plan_length - 1; i++) {
    ant->current_plan[i] = ant->current_plan[i + 1];
  }
  ant->plan_length--;
}

void ant_clear_plan(Ant *ant) {
  if (ant)
    ant->plan_length = 0;
}
