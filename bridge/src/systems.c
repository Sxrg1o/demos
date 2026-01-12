#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "logic.h"

#define STEPS 10

const Vector2 gravity = {0, 981.0f};
const float damping = 0.99f;

void update_physics(World* world, float delta_time) {
    for(int i = 0; i < world->node_count; i++) {
        if(world->nodes[i].inv_mass == 0.0f) continue;
        Vector2 vel = Vector2Scale(Vector2Subtract(world->nodes[i].position, world->nodes[i].prev_position), damping);
        world->nodes[i].acceleration = gravity;
        world->nodes[i].prev_position = world->nodes[i].position;
        world->nodes[i].position = Vector2Add(Vector2Scale(world->nodes[i].acceleration, delta_time*delta_time), 
            Vector2Add(world->nodes[i].position, vel));

        world->nodes[i].acceleration = Vector2Zero();
    }

    for(int n = 0; n < STEPS; n++) {
        for(int i = 0; i < world->link_count; i++) {
            Vector2 delta = Vector2Subtract(world->links[i].b->position, world->links[i].a->position);
            float distance = Vector2Length(delta);

            if(distance < 0.0001f) continue;

            float diff = (distance - world->links[i].ideal_length) / distance;
            float force = world->links[i].a->material->stiffness;

            if(diff < 0) force *= world->links[i].a->material->compression;

            float w_total = world->links[i].a->inv_mass + world->links[i].b->inv_mass;

            if(w_total < 0.00001f) continue;

            Vector2 correction_vector = Vector2Scale(delta, diff * force);

            world->links[i].a->position = Vector2Add(world->links[i].a->position,
                Vector2Scale(correction_vector, world->links[i].a->inv_mass / w_total));
            world->links[i].b->position = Vector2Subtract(world->links[i].b->position,
                Vector2Scale(correction_vector, world->links[i].b->inv_mass / w_total));
        }
    }

    for(int i = 0; i < world->node_count; i++) {
        // TODO: Better collision system, rn just with floor 
        if(world->nodes[i].position.y + world->nodes[i].material->radius > 900.0f) {
            world->nodes[i].position.y = 900.0f - world->nodes[i].material->radius;
            float vel_x = world->nodes[i].position.x - world->nodes[i].prev_position.x;
            world->nodes[i].prev_position.x = world->nodes[i].position.x - 
                (vel_x * (1.0f - world->nodes[i].material->friction));
        }

        if(world->nodes[i].position.x + world->nodes[i].material->radius > 1600.0f) {
            world->nodes[i].position.x = 1600.0f - world->nodes[i].material->radius;
            float vel_y = world->nodes[i].position.y - world->nodes[i].prev_position.y;
            world->nodes[i].prev_position.y = world->nodes[i].position.y -
                (vel_y * (1.0f - world->nodes[i].material->friction));
        }
    }

    for(int i = 0; i < world->link_count; i++) {
        float max_length = world->links[i].ideal_length * world->links[i].a->material->tensile_strength;
        float distance = Vector2Distance(world->links[i].a->position, world->links[i].b->position);
        if(distance > max_length) {
            destroy_link(world, i);
            i--;
        }
    }
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