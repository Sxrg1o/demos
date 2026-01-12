#ifndef SIMULATION_H
#define SIMULATION_H

#include "../entities/ant.h"
#include "ant_world.h"

// Simulation
void system_update_ant(Ant *ant, World *world);

#endif // SIMULATION_H