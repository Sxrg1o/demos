#ifndef ANT_H
#define ANT_H

#include "../logic/ant_math.h"
#include "resource.h"
#include <stdbool.h>

// ANT_RADIUS define rango de vista y olfato
#define ANT_RADIUS 10

typedef enum {
  RETURNING,
  EXPLORING,
  HARVESTING,
  RECRUITING,
  BUILDING,
} AntState;

typedef struct {
  int id;
  AntVector position;
  AntVector vector_to_nest;
  AntState state;
  bool is_carrying;
  Resource carried_resource;
} Ant;

// Lifecycle
void ant_init(Ant *ant, int ant_id, AntVector pos, AntVector nest_pos);
void ant_free(Ant *ant);

#endif // ANT_H
