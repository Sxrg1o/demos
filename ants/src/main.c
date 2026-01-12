#include "system/ant_world.h"
#include "system/generation.h"
#include "system/render.h"
#include <raylib.h>

int main() {
  int factor = 10;
  int W = 80 * factor;
  int H = 60 * factor;
  int MAX_RESOURCES = 10;

  World world;
  world_init(&world, W, H);
  gen_world(&world, MAX_RESOURCES);

  InitWindow(W, H, "Ants");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    render_world(&world);
    EndDrawing();
  }

  CloseWindow();
  return 0;
}
