#include "render.h"

void render_scene(const World *world, const Ant *ants, int ant_count,
                  const Resource *resources, int resource_count,
                  uint32_t *pixels, int width, int height) {
  if (!world || !pixels) {
    return;
  }

  // 1. Draw Environment (Sky/Earth)
  int w = world->width;
  int h = world->height;

  // Validate dimensions to prevent overflow if buffer doesn't match
  int render_w = (w < width) ? w : width;
  int render_h = (h < height) ? h : height;

  for (int y = 0; y < render_h; y++) {
    for (int x = 0; x < render_w; x++) {
      // true = occupied (Earth), false = empty (Sky)
      bool is_earth = world->grid[y][x];
      pixels[y * width + x] = is_earth ? COLOR_EARTH : COLOR_SKY;
    }
  }

  // 2. Draw Resources
  if (resources) {
    for (int i = 0; i < resource_count; i++) {
      Resource r = resources[i];
      if (r.position.x >= 0 && r.position.x < render_w && r.position.y >= 0 &&
          r.position.y < render_h) {

        uint32_t color = COLOR_OTHER;
        switch (r.type) {
        case RESOURCE_FOOD:
          color = COLOR_FOOD;
          break;
        case RESOURCE_STRUCTURAL:
          color = COLOR_STRUCTURE;
          break;
        default:
          color = COLOR_OTHER;
          break;
        }

        pixels[r.position.y * width + r.position.x] = color;
      }
    }
  }

  // 3. Draw Ants
  if (ants) {
    for (int i = 0; i < ant_count; i++) {
      Ant a = ants[i];
      if (a.life > 0 && a.position.x >= 0 && a.position.x < render_w &&
          a.position.y >= 0 && a.position.y < render_h) {

        pixels[a.position.y * width + a.position.x] = COLOR_ANT;
      }
    }
  }
}
