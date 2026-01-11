#ifndef SIMULATION_H
#define SIMULATION_H

#include "../entities/ant.h"
#include "ant_world.h"

// LocalView
int view_init(LocalView *view, int radius);
void view_free(LocalView *view);
void view_populate(LocalView *view, const World *world, Position center);

// Simulation
void system_update_ant(Ant *ant, World *world);
bool system_try_move(Ant *ant, World *world, ActionType action);

#endif // SIMULATION_H