#ifndef RESOURCE_H
#define RESOURCE_H

#include "../logic/ant_math.h"
#include <stdint.h>
typedef enum {
  RESOURCE_DIRT,
  RESOURCE_FOOD,
  RESOURCE_RESERVOIR
} ResourceType;

typedef struct {
  int value;
  int weight;
  uint8_t variant; // 0-255 for visual noise
  ResourceType type;
} Resource;

void resource_init(Resource *r, int value, int weight, ResourceType type);
void resource_free(Resource *r);

Resource resource_dirt();
Resource resource_food();
Resource resource_reservoir();

#endif // RESOURCE_H
