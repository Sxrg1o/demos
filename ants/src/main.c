#include "system/ant_world.h"
#include "system/generation.h"
#include "system/render.h"
#include <raylib.h>
#include <time.h>

int main() {
  int factor = 10;
  int W = 80 * factor;
  int H = 60 * factor;

  World world;
  world_init(&world, W / CELL_SIZE, H / CELL_SIZE);
  gen_world(&world, (unsigned int)time(NULL));

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
