#include "resource.h"
#include <assert.h>
#include <string.h>

void resource_free(Resource *r, ReturnCode *rc) {
  assert(rc);

  if (!r) {
    *rc = RC_NULL_VALUE_ERROR;
    return;
  }

  memset(r, 0, sizeof(*r));
  *rc = RC_SUCCESS;
}
