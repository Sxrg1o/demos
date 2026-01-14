#ifndef SIMULATION_H
#define SIMULATION_H

#include "ant_math.h"
#include "ant_world.h"

// Entities Management
bool entity_spawn_ant(World *w, Position p);
bool entity_kill_ant(int ant_id, World *w);
bool entity_spawn_food(World *w, Position p, int radius);

// Planification
void ant_think(int ant_id, World *w);

// Update helpers
void update_world(World *w);       // update all ants sequentially

// Simulation
void system_update_ant(int ant_id, World *world);

#endif // SIMULATION_H