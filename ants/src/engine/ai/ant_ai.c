#include "ant_ai.h"
#include <stdbool.h>

// --- Helper Functions ---

static bool can_move_to(World *w, AntVector pos) {
  if (!world_in_bounds(w, pos)) {
    return false;
  }
  return !world_is_occupied(w, pos);
}

static void move_ant_to(Ant *a, World *w, AntVector new_pos) {
  world_vacate_cell(w, a->position);

  Cell c = world_get_cell(w, new_pos);
  c.type = CELL_ANT;

  world_occupy_cell(w, new_pos, c);
  a->position = new_pos;
}

// --- Actions ---

void ant_lift(Ant *a, World *w, AntVector from) {
  if (a->is_carrying) {
    return;
  }
  if (!world_in_bounds(w, from)) {
    return;
  }

  Cell c = world_get_cell(w, from);
  if (c.type == CELL_RESOURCE) {
    a->is_carrying = true;
    a->carried_resource = c.resource;
    world_vacate_cell(w, from);
  }
}

void ant_drop(Ant *a, World *w, AntVector to) {
  if (!a->is_carrying) {
    return;
  }
  if (!can_move_to(w, to)) {
    return;
  }

  Cell c = world_get_cell(w, to);
  c.type = CELL_RESOURCE;
  c.resource = a->carried_resource;

  world_occupy_cell(w, to, c);
  a->is_carrying = false;
}

void ant_go(Ant *a, World *w, AntVector target) {
  int dx = (target.x > a->position.x) - (target.x < a->position.x);
  int dy = (target.y > a->position.y) - (target.y < a->position.y);

  if (dx == 0 && dy == 0)
    return;

  AntVector candidates[5];
  int count = 0;

  // 1. Direct Move
  candidates[count++] = (AntVector){(double)dx, (double)dy};

  // 2. Alternatives based on direction
  if (dx != 0 && dy != 0) {
    // Diagonal movement: Try cardinal components
    candidates[count++] = (AntVector){(double)dx, 0};
    candidates[count++] = (AntVector){0, (double)dy};
    candidates[count++] = (AntVector){(double)dx, -(double)dy};
    candidates[count++] = (AntVector){-(double)dx, (double)dy};
  } else if (dx != 0) {
    // Horizontal movement: Try diagonals
    candidates[count++] = (AntVector){(double)dx, 1};
    candidates[count++] = (AntVector){(double)dx, -1};
    candidates[count++] = (AntVector){0, 1};
    candidates[count++] = (AntVector){0, -1};
  } else {
    // Vertical movement: Try diagonals
    candidates[count++] = (AntVector){1, (double)dy};
    candidates[count++] = (AntVector){-1, (double)dy};
    candidates[count++] = (AntVector){1, 0};
    candidates[count++] = (AntVector){-1, 0};
  }

  // Execute first valid move
  for (int i = 0; i < count; i++) {
    AntVector offset = candidates[i];
    AntVector next_pos = {a->position.x + offset.x, a->position.y + offset.y};

    // Interaction: Pick up resource if blocking
    Cell cell = world_get_cell(w, next_pos);
    if (!a->is_carrying && cell.type == CELL_RESOURCE) {
      ant_lift(a, w, next_pos);
      return;
    }

    if (can_move_to(w, next_pos)) {
      move_ant_to(a, w, next_pos);
      return;
    }
  }
}

void ant_update(Ant *a, World *w) {
  switch (a->state) {
  case EXPLORING:
    break;
  case RETURNING:
    break;
  case HARVESTING:
    break;
  case RECRUITING:
    break;
  case BUILDING:
    break;
  }
}
