#ifndef PATHFINDING_H
#define PATHFINDING_H

#include "ant_world.h"

int pf_astar(World *w, Position start, Position goal, Position *out_path, int max_len);
int pf_is_walkable(const World *w, Position p);

#endif // PATHFINDING_H
