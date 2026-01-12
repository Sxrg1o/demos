#ifndef GENERATION_H
#define GENERATION_H

#include "../entities/resource.h"
#include "ant_world.h"

int generate_world_content(World *world, Resource *resources,
                           int max_resources);

#endif // GENERATION_H
