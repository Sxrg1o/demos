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
