#include <raylib.h>
#include <stdio.h>

#include "logic.h"
#include "systems.h"

#define PHYS_DT (1.0f / 60.0f)
#define PHYS_STEPS 8

int main(void) {
    const int mult = 100;
    const int screen_width = 16 * mult;
    const int screen_height = 9 * mult;

    InitWindow(screen_width, screen_height, "bridge");

    World world = {0};
    BMaterial material_list[MATERIALS_COUNT];
    init_world(&world);
    init_materials(material_list);

    double current_time = GetTime();
    double accumulator = 0.0;
    int last_node_idx = -1;
    int selected_node_idx = -1;

    while(!WindowShouldClose()) {
        double new_time = GetTime();
        double frame_time = new_time - current_time;
        current_time = new_time;

        if(frame_time > 0.25) frame_time = 0.25;

        if(IsKeyPressed(KEY_W)) world.state.material_type = WOOD;
        if(IsKeyPressed(KEY_S)) world.state.material_type = SPRING;
        if(IsKeyPressed(KEY_R)) world.state.material_type = ROPE;
        if(IsKeyPressed(KEY_T)) world.state.material_type = STONE;
        if(IsKeyPressed(KEY_F)) world.state.fix = !world.state.fix;
        if(IsKeyPressed(KEY_SPACE)) world.state.building = !world.state.building;

        Vector2 mouse_pos = GetMousePosition();

        int hovered_node_idx = -1;
        float selection_radius = 15.0f;

        for(int i = 0; i < world.node_count; i++) {
            if(CheckCollisionPointCircle(mouse_pos, world.nodes[i].position, selection_radius)) {
                hovered_node_idx = i;
                break;
            }
        }

        if(world.state.building) {
            accumulator = 0.0;
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                int current_target_idx = -1;

                if(hovered_node_idx != -1) {
                    current_target_idx = hovered_node_idx;
                } else {
                    if(world.node_count < MAX_NODES) {
                        create_node(&world, mouse_pos, material_list);
                        current_target_idx = world.node_count - 1;
                    } else {
                        current_target_idx = -1;
                    }
                }

                Node* curr_node = &world.nodes[current_target_idx];
                BMaterial* stone_material = &material_list[STONE];
    
                if(last_node_idx != -1 && last_node_idx != current_target_idx) {
                    Node* prev_node = &world.nodes[last_node_idx];

                    if(prev_node->material != stone_material && curr_node->material != stone_material 
                        && !link_exists(&world, prev_node, curr_node) && world.link_count < MAX_LINKS) {
                        create_link(&world, prev_node, curr_node);
                    }
                }
                if(curr_node->material == stone_material) {
                    last_node_idx = -1;
                } else {
                    last_node_idx = current_target_idx;
                }
            }

            if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) last_node_idx = -1;
        } else {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                selected_node_idx = -1;

                for(int i = 0; i < world.node_count; i++) {
                    if(CheckCollisionPointCircle(mouse_pos, world.nodes[i].position, selection_radius)) {
                        selected_node_idx = i;
                        break;
                    }
                }

                if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    selected_node_idx = -1;
                }
            }

            accumulator += frame_time;

            while(accumulator >= PHYS_DT) {
                for(int i = 0; i < PHYS_STEPS; i++) update_physics(&world, PHYS_DT, selected_node_idx, mouse_pos);
                accumulator -= PHYS_DT;
            }
        } 

        BeginDrawing();

            ClearBackground(RAYWHITE);

            update_render(&world);

            if(world.state.building) {
                DrawText("Building", 10, 10, 20, DARKGREEN);
                if(last_node_idx != -1) {
                    Vector2 start_pos = world.nodes[last_node_idx].position;
                    Vector2 end_pos = (hovered_node_idx != -1) ? world.nodes[hovered_node_idx].position : mouse_pos;
                    DrawLineEx(start_pos, end_pos, 2.0f, LIGHTGRAY);
                    DrawCircleLinesV(start_pos, 8.0f, ORANGE);
                }
                if(hovered_node_idx != -1) {
                    DrawCircleLinesV(world.nodes[hovered_node_idx].position, 12.0f, GREEN);
                    DrawText("Link", world.nodes[hovered_node_idx].position.x + 10, world.nodes[hovered_node_idx].position.y - 20, 10, DARKGREEN);
                }
                Color node_color = (world.node_count >= MAX_NODES * 0.9f) ? RED : DARKGRAY;
                Color link_color = (world.link_count >= MAX_LINKS * 0.9f) ? RED : DARKGRAY;

                DrawText(TextFormat("Nodes: %d / %d", world.node_count, MAX_NODES), 10, screen_height - 60, 20, node_color);
                DrawText(TextFormat("Links: %d / %d", world.link_count, MAX_LINKS), 10, screen_height - 30, 20, link_color);
                if(world.state.fix) {
                    DrawText("Static node [F]", 10, 70, 20, RED);
                } else {
                    DrawText("Dynamic node [F]", 10, 70, 20, BLACK);
                }
                const char* matName = material_list[world.state.material_type].name;
                DrawText(TextFormat("Material: %s [W, R, S]", matName), 10, 40, 20, DARKGRAY);
            } else {
                DrawText("Simulating", 10, 10, 20, BLUE);
            }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}