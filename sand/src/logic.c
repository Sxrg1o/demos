#include <stdlib.h>

#include "logic.h"

static void init_families(World* w) {
    w->families[FAM_DEFAULT] = (MaterialFamily){9999.0f, 9999.0f, 9999.0f, 1.0f, 1.0f};
    
    w->families[FAM_WATER].melting_point = 0.0f;
    w->families[FAM_WATER].boiling_point = 100.0f;
    w->families[FAM_WATER].ignite_temperature = 9999.0f;
    w->families[FAM_WATER].density_gas_mult = 0.05f;
    w->families[FAM_WATER].density_liq_mult = 1.0f;

    w->families[FAM_STONE].melting_point = 1200.0f;
    w->families[FAM_STONE].boiling_point = 3000.0f;
    w->families[FAM_STONE].ignite_temperature = 9999.0f;
    w->families[FAM_STONE].density_gas_mult = 0.5f;
    w->families[FAM_STONE].density_liq_mult = 0.9f;

    w->families[FAM_ORGANIC].melting_point = 9999.0f; 
    w->families[FAM_ORGANIC].boiling_point = 9999.0f;
    w->families[FAM_ORGANIC].ignite_temperature = 250.0f;
    w->families[FAM_ORGANIC].density_gas_mult = 0.1f;
    w->families[FAM_ORGANIC].density_liq_mult = 1.0f;
}

void init_world(World* w) {
    w->width = WORLD_WIDTH;
    w->height = WORLD_HEIGHT;
    w->global_clock = 0;
    w->cells = (Cell*)malloc(sizeof(Cell) * w->width * w->height);
    init_families(w);
    for(int i = 0; i < w->width * w->height; i++) {
        set_cell_empty(&w->cells[i]);
    }
}

void destroy_world(World* w) {
    if(w->cells) free(w->cells);
}

Cell* get_cell(World* w, int x, int y) {
    if(x < 0 || x >= w->width || y < 0 || y >= w->height) return NULL;
    return &w->cells[y * w->width + x];
}

void set_cell_empty(Cell* c) {
    c->is_empty = true;
    c->state = GAS;
    c->density = 0.0f;
    c->temperature = 20.0f;
    c->family_idx = FAM_DEFAULT;
    c->base_color = BLANK;
    c->life = 0;
    c->payload = 0;
}

void set_cell(Cell* c, ElementState st, Color col, float dens, float visc, uint8_t fam, uint32_t flags, float variance) {
    c->is_empty = false;
    c->state = st;
    c->density = dens;
    c->dispersion = visc;
    c->family_idx = fam;
    c->chem_flags = flags;
    c->temperature = 20.0f;
    c->base_temp = 20.0f;
    c->acid_resist = 1.0f;
    c->life = 0;
    c->payload = 0;

    if(variance > 0.0f) {
        float factor = 1.0f + ((float)GetRandomValue(-100, 100) / 100.0f) * variance;
        c->base_color = ColorBrightness(col, factor - 1.0f);
    } else {
        c->base_color = col;
    }
}