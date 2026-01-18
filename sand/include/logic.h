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
    float melting_point;
    float boiling_point;
    float ignite_temperature;
    float density_gas_mult;
    float density_liq_mult;
} MaterialFamily;

typedef struct {
    uint8_t state;
    float density;
    int dispersion;     // Like viscosity (pixel/frame)
    float base_temp;

    float temperature;
    float heat_conductivity;
    uint8_t family_idx;
    
    uint32_t chem_flags;
    float acid_resist;

    Color base_color;
    float color_variance; 

    uint8_t life;       // Fire/Smoke or corrosive materials
    uint8_t payload;    // For color herency

    bool is_empty;
} Cell;

#define WORLD_WIDTH 480
#define WORLD_HEIGHT 270
#define MAX_FAMILIES 16

typedef struct {
    Cell* cells;
    MaterialFamily families[MAX_FAMILIES];
    int family_count;
    uint8_t global_clock;
    int width;
    int height;
} World;

#define FAM_DEFAULT 0
#define FAM_WATER 1
#define FAM_STONE 2
#define FAM_ORGANIC 3

void init_world(World*);
void destroy_world(World*);
Cell* get_cell(World*, int, int);

void set_cell(Cell*, ElementState, Color, float, float, uint8_t, uint32_t, float);
void set_cell_empty(Cell*);