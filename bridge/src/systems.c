#include <raylib.h>
#include <raymath.h>
#include <math.h>

#include "logic.h"

#define STEPS 10

const Vector2 gravity = {0, 9.81f};
const float damping = 0.99f;

void update_physics(World* world, float delta_time) {
    for(int i = 0; i < world->node_count; i++) {
        if(world->nodes[i].inv_mass == 0.0f) continue;
        Vector2 vel = Vector2Scale(Vector2Subtract(world->nodes[i].position, world->nodes[i].prev_position), damping);
        world->nodes[i].acceleration = gravity;
        world->nodes[i].prev_position = world->nodes[i].position;
        world->nodes[i].position = Vector2Add(Vector2Scale(world->nodes[i].acceleration, delta_time*delta_time), 
            Vector2Add(world->nodes[i].position, vel));

        Vector2 movement = Vector2Subtract(world->nodes[i].position, world->nodes[i].prev_position);
        float move_len = Vector2Length(movement);
        float max_move = 20.0f;

        if(move_len > max_move) {
            movement = Vector2Scale(movement, max_move / move_len);
            world->nodes[i].position = Vector2Add(world->nodes[i].prev_position, movement);
        }
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
        Node* rock = &world->nodes[i];
        
        if(strcmp(rock->material->name, "Stone") != 0) continue; 

        for(int j = 0; j < world->link_count; j++) {
            Link* link = &world->links[j];

            if(link->a == rock || link->b == rock) continue;

            Vector2 ab = Vector2Subtract(link->b->position, link->a->position);
            Vector2 ap = Vector2Subtract(rock->position, link->a->position);
            
            float ab_len_sq = Vector2LengthSqr(ab);
            if(ab_len_sq == 0.0f) continue;

            float t = Vector2DotProduct(ap, ab) / ab_len_sq;
            if(t < 0.0f) t = 0.0f;
            if(t > 1.0f) t = 1.0f;

            Vector2 closest = Vector2Add(link->a->position, Vector2Scale(ab, t));
            Vector2 dist_vec = Vector2Subtract(rock->position, closest);
            float dist = Vector2Length(dist_vec);
            float min_dist = rock->material->radius + link->a->material->radius;

            if(dist < min_dist) {
                Vector2 normal = Vector2Zero();
                if(dist > 0.0001f) {
                    normal = Vector2Scale(dist_vec, 1.0f / dist);
                } else {
                    normal = (Vector2){0, -1};
                }

                float penetration = min_dist - dist;
                float w_rock = rock->inv_mass;
                float w_link_a = link->a->inv_mass;
                float w_link_b = link->b->inv_mass;
                float w_total = w_rock + w_link_a + w_link_b;

                if(w_total == 0.0f) continue;

                if(!rock->fixed) {
                    float move_rock = penetration * (w_rock / w_total);
                    rock->position = Vector2Add(rock->position, Vector2Scale(normal, move_rock));
                }

                float move_link = penetration * ((w_link_a + w_link_b) / w_total);
                float force_a = (1.0f - t) * move_link;
                float force_b = t * move_link;

                if(!link->a->fixed) {
                    link->a->position = Vector2Subtract(link->a->position, Vector2Scale(normal, force_a));
                }
                if(!link->b->fixed) {
                    link->b->position = Vector2Subtract(link->b->position, Vector2Scale(normal, force_b));
                }
            }
        }
    }

    for(int i = 0; i < world->node_count; i++) {
        if(world->nodes[i].position.y + world->nodes[i].material->radius > GetScreenHeight()) {
            world->nodes[i].position.y = GetScreenHeight() - world->nodes[i].material->radius;
            float vel_x = world->nodes[i].position.x - world->nodes[i].prev_position.x;
            world->nodes[i].prev_position.x = world->nodes[i].position.x - 
                (vel_x * (1.0f - world->nodes[i].material->friction));
        }

        if(world->nodes[i].position.x + world->nodes[i].material->radius > GetScreenWidth()) {
            world->nodes[i].position.x = GetScreenWidth() - world->nodes[i].material->radius;
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