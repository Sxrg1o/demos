#include <raylib.h>

#include "systems.h"
#include "logic.h"

int main(void) {
    const int mult = 100;
    const int screen_width = 16 * mult;
    const int screen_height = 9 * mult;

    InitWindow(screen_width, screen_height, "sand");

    World world = {0};
    init_world(&world);
    double current_time = GetTime();

    while(!WindowShouldClose()) {
    
        
        BeginDrawing();
        

        EndDrawing();
    }

    CloseWindow();
    return 0;
}