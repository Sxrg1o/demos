#pragma once

#include <raylib.h>
#include <stdint.h>

typedef enum {
    SOLID,
    LIQUID,
    POWDER,
    GAS,
    FIRE
} ElementState;

typedef enum {
    NONE = 0,
    FUEL = (1 << 0),
    OXIDIZER = (1 << 1),
    ACID = (1 << 2),
    BASE = (1 << 3),
    WATER = (1 << 4),
    TOXIC = (1 << 5),
    ORGANIC = (1 << 6),
    SOLUBLE = (1 << 7)
} ChemFlags;

typedef struct {
    int solid_id;
    int liquid_id;
    int gas_id;
    float melting_point;
    float boiling_point;
    float ignite_temperature;
} MaterialFamily;

typedef struct {
    const char* name;

    ElementState state;
    float density;
    int dispersion;     // Like viscosity (pixel/frame)

    float base_temp;
    float heat_conductivity;
    int family_id;

    uint32_t chem_flags;
    float acid_resist;

    Color base_color;
    float color_variance;    
} ElementDef;

typedef struct {
    uint8_t type;
    uint8_t life;       // Fire/Smoke or corrosive materials
    uint8_t payload;    // For color herency
    float temperature;
} Cell;

#define WORLD_WIDTH 480
#define WORLD_HEIGHT 270

typedef struct {
    Cell cells[WORLD_WIDTH * WORLD_HEIGHT];
    uint8_t global_clock;
    ElementDef elements[80];
    MaterialFamily families[16];
    int family_count;
    int element_count;
} World;

void init_world(World*);
void create_pixel(World*);