#ifndef RESOURCE_H
#define RESOURCE_H

#include "../config/error.h"

typedef enum {
  OTHER,
  STRUCTURAL,
  FOOD,
} ResourceType;

typedef struct {
  int value;
  int weight;
  ResourceType type;
} Resource;

void resource_free(Resource *r, ReturnCode *rc);

#endif // RESOURCE_H
