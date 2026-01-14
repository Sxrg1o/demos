#include "render.h"
#include "ant_world.h"
#include <raylib.h>

void render_world(const World *w) {
  ClearBackground(GetColor(COLOR_SKY));

  for (int x = 0; x < w->width; x++) {
    for (int y = 0; y < w->height; y++) {
      Color cell_color = GetColor(COLOR_SKY);
      Cell cell = w->grid[y * w->width + x];
      switch (cell.type) {
      case CELL_EMPTY:
        break;
      case CELL_ANT:
        cell_color = GetColor(COLOR_ANT);
        break;
      case CELL_NEST:
        cell_color = GetColor(COLOR_NEST);
        break;
      case CELL_RESOURCE:
        if (cell.resource.type == RESOURCE_DIRT) {
          cell_color = GetColor(COLOR_DIRT);
        }
        if (cell.resource.type == RESOURCE_FOOD) {
          cell_color = GetColor(COLOR_FOOD);
        }
        break;
      case CELL_OUT_OF_BOUNDS:
        cell_color = DARKGRAY;
        break;
      }
      DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE,
                    cell_color);
    }
  }
}
