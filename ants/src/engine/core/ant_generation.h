#ifndef ANT_GENERATION_H
#define ANT_GENERATION_H

#include "../world/ant_world.h"

void gen_world(World *world, unsigned int seed);
void gen_flat_world(World *world, int empty_width);

#endif // ANT_GENERATION_H
