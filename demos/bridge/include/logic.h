#pragma once

#include <stdbool.h>
#include <raymath.h>
#include <string.h>

typedef struct {
    char name[10];
    float density;              // Just mass calculations
    float stiffness;            // For material physics
    float tensile_strength;     // Threshold for link breaking
    float compression;          // For material physics
    float friction;             // For collisions
    float radius;               // se que no es del material pero xd
} BMaterial;

typedef struct {
    Vector2 position;
    Vector2 prev_position;
    Vector2 acceleration;
    float inv_mass;
    bool fixed;
    BMaterial* material;
} Node;

typedef struct {
    Node* a;
    Node* b;
    float ideal_length;
} Link;

#define MATERIALS_COUNT 3

typedef enum {
    WOOD,
    ROPE,
    SPRING
} MaterialType;

typedef struct {
    MaterialType material_type;
    bool fix;
    bool building;
} State;

typedef struct {
    Node nodes[72];
    Link links[200];
    int node_count;
    int link_count;
    State state;
} World;

void init_world(World*);
void init_materials(BMaterial*);
void create_node(World*, Vector2, BMaterial*);
void create_link(World*, Node*, Node*);