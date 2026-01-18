#include <raylib.h>

#include "logic.h"
#include "systems.h"

int main(void) {
    const int mult = 3;
    const int screen_width = WORLD_WIDTH * mult;
    const int screen_height = WORLD_HEIGHT * mult;

    InitWindow(screen_width, screen_height, "sand");
    SetTargetFPS(60);

    World world = {0};
    init_world(&world);

    RenderTexture2D target = LoadRenderTexture(WORLD_WIDTH, WORLD_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);

    int brush = 1;

    while(!WindowShouldClose()) {
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 m = GetMousePosition();
            int cx = (int)(m.x / mult);
            int cy = (int)(m.y / mult);
            int r = 2;

            for(int y=-r; y<=r; y++) {
                for(int x=-r; x<=r; x++) {
                    if(x*x + y*y <= r*r) {
                        Cell* c = get_cell(&world, cx+x, cy+y);
                        if(c) {
                            if(brush==1) set_cell(c, POWDER, GOLD, 50.0f, 20.0f, FAM_STONE, NONE, 0.1f);
                            if(brush==2) set_cell(c, LIQUID, BLUE, 10.0f, 20.0f, FAM_WATER, WATER, 0.0f);
                            if(brush==3) set_cell(c, SOLID, DARKGRAY, 100.0f, 20.0f, FAM_STONE, NONE, 0.0f);
                            if(brush==4) set_cell(c, SOLID, BROWN, 40.0f, 20.0f, FAM_ORGANIC, FUEL|ORGANIC, 0.1f);
                            if(brush==5) {
                                set_cell(c, FIRE, ORANGE, -1.0f, 800.0f, FAM_DEFAULT, FUEL, 0.2f);
                                c->life = 100;
                            }
                            if(brush==0) set_cell_empty(c);
                        }
                    }
                }
            }
        }

        if(IsKeyPressed(KEY_ONE)) brush = 1;
        if(IsKeyPressed(KEY_TWO)) brush = 2;
        if(IsKeyPressed(KEY_THREE)) brush = 3;
        if(IsKeyPressed(KEY_FOUR)) brush = 4;
        if(IsKeyPressed(KEY_FIVE)) brush = 5;
        if(IsKeyPressed(KEY_ZERO)) brush = 0;

        update_thermodynamics(&world);
        update_chemistry(&world);
        update_position(&world);

        BeginTextureMode(target);
            update_render(&world);
        EndTextureMode();

        BeginDrawing();
            DrawTexturePro(target.texture,
                (Rectangle){0, 0, (float)target.texture.width, (float)-target.texture.height},
                (Rectangle){0, 0, (float)screen_width, (float)screen_height},
                (Vector2){0, 0}, 0.0f, WHITE);
            
            DrawFPS(10, 10);
            DrawText("1: Sand  2: Water  3: Stone  4: Wood  5: Fire", 10, 30, 20, RAYWHITE);
        EndDrawing();
    }

    destroy_world(&world);
    UnloadRenderTexture(target);
    CloseWindow();
    return 0;
}