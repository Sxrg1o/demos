#include <raylib.h>

#include "ex.h"

int main(void) {
    const int screen_width = 1600;
    const int screen_height = 900;

    InitWindow(screen_width, screen_height, "bridge");

    State world_state;

    while(!WindowShouldClose()) {
        
    }

    CloseWindow();
    return 0;
}