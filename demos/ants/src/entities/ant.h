#ifndef ANT_H
#define ANT_H

#include "../config/error.h"
#include "../system/ant_math.h"
#include "resource.h"
#include <stdbool.h>

typedef struct {
  int carry_weight_g;
  int weight_g;
  int life;
  bool carring_food;
} Ant;

void resource_carry(const Ant *ant, const Resource *resource, ReturnCode *rc);
void resource_consume(const Ant *ant, const Resource *resource, ReturnCode *rc);
void resource_drop(const Ant *ant, const Resource *resource, const Position pos,
                   ReturnCode *rc);
void ant_free(Ant *ant, ReturnCode *rc);

#endif // ANT_H