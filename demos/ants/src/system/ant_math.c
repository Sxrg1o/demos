#include "ant_math.h"

Position position_add(const Position a, const Position b) {
  return (Position){a.x + b.x, a.y + b.y};
}