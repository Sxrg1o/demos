#include "engine/core/ant_generation.h"
#include "engine/core/ant_simulation.h"
#include "engine/render/ant_render.h"
#include <raylib.h>

int main() {
  int factor = 10;
  int W = 80 * factor;
  int H = 60 * factor;

  World world;
  world_init(&world, W / CELL_SIZE, H / CELL_SIZE);
  gen_flat_world(&world, 5);

  InitWindow(W, H, "Ants");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
      Vector2 m = GetMousePosition();
      int gx = (int)(m.x / CELL_SIZE);
      int gy = (int)(m.y / CELL_SIZE);
      entity_spawn_ant(&world, (AntVector){gx, gy});
    }

    BeginDrawing();
    render_world(&world);
    EndDrawing();
    system_update_world(&world);
  }

  CloseWindow();
  world_free(&world);
  return 0;
}
