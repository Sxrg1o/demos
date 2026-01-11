#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "logic.h"

const Vector2 gravity = {0, -9.81};

void update_physics(World* world, float delta_time) {

}

void update_render(World* world) {
    for(int i = 0; i < world->link_count; i++) {
        Node* a = world->links[i].a;
        Node* b = world->links[i].b;
        DrawLineEx(a->position, b->position, a->material->radius, a->material->color);
    }

    for(int i = 0; i < world->node_count; i++) {
        Vector2 node_position = world->nodes[i].position;
        BMaterial* material = world->nodes[i].material;
        DrawCircleV(node_position, material->radius, material->color);
        if(world->nodes[i].fixed) DrawCircleV(node_position, 2.0f, RED);
    }
}