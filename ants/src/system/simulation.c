#include "simulation.h"
#include "ant_world.h"

void system_update_ant(Ant *ant, World *world) {
  if (!ant || !world) {
    return;
  }
  // TODO:
  // if cannot execute action: replan
  // execute action - update world
}
