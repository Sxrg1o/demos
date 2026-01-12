#ifndef ANT_MATH_H
#define ANT_MATH_H

typedef struct {
  int x;
  int y;
} Position;

Position position_add(const Position a, const Position b);

#endif // ANT_MATH_H