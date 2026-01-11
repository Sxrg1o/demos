#ifndef RESOURCE_H
#define RESOURCE_H

#include "../system/ant_math.h"

typedef enum {
  RESOURCE_OTHER,
  RESOURCE_STRUCTURAL,
  RESOURCE_FOOD,
} ResourceType;

typedef struct {
  Position position;
  int value;
  int weight;
  ResourceType type;
} Resource;

void resource_init(Resource *r, Position pos, int value, int weight,
                   ResourceType type);
void resource_free(Resource *r);

#endif // RESOURCE_H
