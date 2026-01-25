#include "render.h"
#include <math.h>
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
        if (cell.resource.type == RESOURCE_RESERVOIR) {
          cell_color = GetColor(COLOR_RESERVOIR);
        }
        break;
      case CELL_OUT_OF_BOUNDS:
        cell_color = DARKGRAY;
        break;
      }

      float p_food = cell.pheromone_to_food;
      float p_home = cell.pheromone_to_home;
      float p_visited = cell.pheromone_visited;

      // clamp pheromone intensities
      if (p_food < 0)
        p_food = 0;
      if (p_home < 0)
        p_home = 0;
      if (p_visited < 0)
        p_visited = 0;
      float max_p = fmaxf(p_food, fmaxf(p_home, p_visited));

      if (max_p > 0.0001f) {
        unsigned char pr = (unsigned char)fminf(p_food * 255.0f, 255.0f);
        unsigned char pg = (unsigned char)fminf(p_visited * 200.0f, 255.0f);
        unsigned char pb = (unsigned char)fminf(p_home * 255.0f, 255.0f);

        Color pher = {pr, pg, pb, 255};
        float t = fminf(max_p, 1.0f);
        Color base = cell_color;
        Color out;
        out.r = (unsigned char)((1.0f - t) * base.r + t * pher.r);
        out.g = (unsigned char)((1.0f - t) * base.g + t * pher.g);
        out.b = (unsigned char)((1.0f - t) * base.b + t * pher.b);
        out.a = 255;
        DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE, out);
      } else {
        DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE,
                      cell_color);
      }
    }
  }
}
