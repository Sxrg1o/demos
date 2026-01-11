#include <raylib.h>
#include <stdio.h>

#include "logic.h"
#include "systems.h"

int main(void) {
    const int screen_width = 1600;
    const int screen_height = 900;

    InitWindow(screen_width, screen_height, "bridge");

    World world = {0};
    BMaterial material_list[MATERIALS_COUNT];
    init_world(&world);
    init_materials(material_list);

    double current_time = GetTime();

    while(!WindowShouldClose()) {
        double new_time = GetTime();
        double delta_time = new_time - current_time;
        current_time = new_time;

        if(IsKeyDown(KEY_W)) world.state.material_type = WOOD;
        if(IsKeyDown(KEY_S)) world.state.material_type = SPRING;
        if(IsKeyDown(KEY_R)) world.state.material_type = ROPE;

        Vector2 mouse_pos = GetMousePosition();

        

        BeginDrawing();

            ClearBackground(RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}