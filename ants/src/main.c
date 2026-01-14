#include "system/ant_world.h"
#include "system/generation.h"
#include "system/render.h"
#include "system/simulation.h"
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
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 m = GetMousePosition();
      int gx = (int)(m.x / CELL_SIZE);
      int gy = (int)(m.y / CELL_SIZE);
      entity_spawn_ant(&world, (Position){gx, gy});
    }

    BeginDrawing();
    render_world(&world);
    EndDrawing();
    update_world(&world);
  }

  CloseWindow();
  world_free(&world);
  return 0;
}
