#ifndef SIMULATION_H
#define SIMULATION_H

#include "../world/ant_world.h"
#include "ant_math.h"

// Entities Management
bool entity_spawn_ant(World *w, AntVector p);
bool entity_spawn_food(World *w, AntVector p, int radius);

// Simulation Step
void system_update_world(World *w);

#endif // SIMULATION_H