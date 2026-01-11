#include "ant.h"
#include <assert.h>

void ant_free(Ant *ant, ReturnCode *rc) {
  assert(rc);
  if (!ant) {
    *rc = RC_NULL_VALUE_ERROR;
    return;
  }

  ant->carry_weight_g = 0;
  ant->weight_g = 0;
  ant->life = 0;
  ant->carring_food = false;

  *rc = RC_SUCCESS;
}
