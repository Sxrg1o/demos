#include "ant.h"

void ant_init(Ant *ant, int ant_id, AntVector pos, AntVector nest_pos) {
  if (!ant)
    return;

  ant->position = pos;
  ant->vector_to_nest = AntVector_add(pos, AntVector_reverse(nest_pos));
  ant->id = ant_id;
  ant->state = RETURNING;
  ant->is_carrying = false;
  ant->carried_resource = (Resource){0};
}

void ant_free(Ant *ant) {
  // Nothing to free yet as there are no dynamic allocations inside Ant
}
