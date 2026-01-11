#include <raylib.h>
#include <string.h>

#include "logic.h"

void init_world(World* world) {
    world->state.material_type = WOOD;
    world->state.fix = true;
    world->state.building = true;
    world->link_count = 0;
    world->node_count = 0;
}

void init_materials(BMaterial* material_list) {
    material_list[WOOD].density = 10.0f;
    material_list[WOOD].stiffness = 1.0f;
    material_list[WOOD].tensile_strength = 1.05f;
    material_list[WOOD].compression = 1.0f;
    material_list[WOOD].friction = 0.5f;
    material_list[WOOD].radius = 5.0f;
    strncpy(material_list[WOOD].name, "Wood", 9);

    material_list[ROPE].density = 2.0f;
    material_list[ROPE].stiffness = 0.8f;
    material_list[ROPE].tensile_strength = 1.5f;
    material_list[ROPE].compression = 0.0f;
    material_list[ROPE].friction = 0.8f;
    material_list[ROPE].radius = 2.0f;
    strncpy(material_list[ROPE].name, "Rope", 9);

    material_list[SPRING].density = 5.0f;
    material_list[SPRING].stiffness = 0.1f;
    material_list[SPRING].tensile_strength = 5.0f;
    material_list[SPRING].compression = 1.0f;
    material_list[SPRING].friction = 0.3f;
    material_list[SPRING].radius = 3.0f;
    strncpy(material_list[SPRING].name, "Spring", 9);
}

void create_node(World* world, Vector2 position, BMaterial* material_list) {
    int idx = world->node_count;
    world->nodes[idx].prev_position = position;
    world->nodes[idx].position = position;

    BMaterial* material = &material_list[world->state.material_type];
    world->nodes[idx].material = material;

    world->nodes[idx].fixed = world->state.fix;

    if(world->state.fix) {
        world->nodes[idx].inv_mass = 0.0f;
    } else {
        world->nodes[idx].inv_mass = 1.0f / (material->density * material->radius * material->radius * PI);
    }

    world->node_count++;
}

void create_link(World* world, Node* a, Node* b) {
    int idx = world->link_count;
    world->links[idx].a = a;
    world->links[idx].b = b;
    world->links[idx].ideal_length = Vector2Distance(b->position, a->position);
    world->link_count++;
}