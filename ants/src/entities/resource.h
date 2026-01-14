#ifndef RESOURCE_H
#define RESOURCE_H

#include "../system/ant_math.h"

typedef enum {
  RESOURCE_DIRT,
  RESOURCE_FOOD,
  RESOURCE_RESERVOIR
} ResourceType;

typedef struct {
  int value;
  int weight;
  ResourceType type;
} Resource;

void resource_init(Resource *r, int value, int weight, ResourceType type);
void resource_free(Resource *r);

#endif // RESOURCE_H
