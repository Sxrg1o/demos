#include "resource.h"
#include <string.h>

void resource_init(Resource *r, int value, int weight, ResourceType type) {
  if (!r) {
    return;
  }
  r->value = value;
  r->weight = weight;
  r->type = type;
}

void resource_free(Resource *r) {
  if (!r) {
    return;
  }
  memset(r, 0, sizeof(*r));
}

Resource resource_dirt() {
  return (Resource){.value = 0, .weight = 1, .type = RESOURCE_DIRT};
}
Resource resource_food() {
  return (Resource){.value = 8, .weight = 5, .type = RESOURCE_FOOD};
}
Resource resource_reservoir() {
  return (Resource){.value = 10, .weight = 5, .type = RESOURCE_RESERVOIR};
}