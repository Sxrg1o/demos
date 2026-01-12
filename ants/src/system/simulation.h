#ifndef SIMULATION_H
#define SIMULATION_H

#include "ant_world.h"

// Planification
void agent_think(const Ant *ant, const World *w);

// Simulation
void system_update_ant(Ant *ant, World *world);

#endif // SIMULATION_H