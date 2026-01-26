#ifndef ANT_AI_H
#define ANT_AI_H

#include "../world/ant_world.h"

// Actions
void ant_drop(Ant *a, World *w, AntVector to);
void ant_lift(Ant *a, World *w, AntVector from);
void ant_go(Ant *a, World *w, AntVector to);

// Update
void ant_update(Ant *a, World *w);

#endif // ANT_AI_H