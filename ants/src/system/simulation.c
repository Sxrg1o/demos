#include "simulation.h"
#include "../entities/ant.h"
#include "ant_math.h"
#include "ant_world.h"
#include <stdlib.h>

// Entities Management
bool entity_spawn_ant(World *w, Position p) {
  if (!w || !world_in_bounds(w, p)) {
    return false;
  }

  if (w->num_ants >= MAX_ANTS) {
    return false;
  }

  if (world_is_occupied(w, p)) {
    return false;
  }

  int id = w->num_ants;
  Ant *ant = &w->ants[id];

  ant_init(ant, p, w->nest_pos);
  Cell cell = w->grid[p.y * w->width + p.x];
  cell.type = CELL_ANT;
  cell.ant_id = id;

  world_occupy_cell(w, p, cell);

  w->num_ants++;
  return true;
}

bool entity_kill_ant(int ant_id, World *w) {
  if (!w || ant_id < 0 || ant_id >= w->num_ants) {
    return false;
  }

  Ant *victim = &w->ants[ant_id];
  if (world_in_bounds(w, victim->position)) {
    Cell *c = &w->grid[victim->position.y * w->width + victim->position.x];
    if (c->type == CELL_ANT && c->ant_id == ant_id) {
      world_vacate_cell(w, victim->position);
    }
  }

  int last_id = w->num_ants - 1;
  if (ant_id != last_id) {
    Ant *last_ant = &w->ants[last_id];
    w->ants[ant_id] = *last_ant;

    Position p = last_ant->position;
    if (world_in_bounds(w, p)) {
      Cell *c = &w->grid[p.y * w->width + p.x];
      if (c->type == CELL_ANT && c->ant_id == last_id) {
        c->ant_id = ant_id;
      }
    }
  }

  w->num_ants--;
  return true;
}

bool entity_spawn_food(World *w, Position p, int radius) {
  if (!w)
    return false;

  bool placed = false;
  int r2 = radius * radius;

  for (int dy = -radius; dy <= radius; dy++) {
    for (int dx = -radius; dx <= radius; dx++) {
      if (dx * dx + dy * dy <= r2) {
        Position target = {p.x + dx, p.y + dy};

        if (world_in_bounds(w, target)) {
          Cell *c = &w->grid[target.y * w->width + target.x];

          c->type = CELL_RESOURCE;
          c->resource.type = RESOURCE_FOOD;
          c->resource.value = 10;
          c->resource.weight = 1;
          placed = true;
        }
      }
    }
  }
  return placed;
}

// Planification
void ant_think(int ant_id, World *w) {
  if (!w || ant_id < 0 || ant_id >= w->num_ants) {
    return;
  }

  Ant *ant = &(w->ants[ant_id]);
  ant_clear_plan(ant);

  /* gather adjacent positions (8-neighborhood) */
  Position adj[8];
  bool all_empty = true;
  int ai = 0;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx == 0 && dy == 0)
        continue;
      adj[ai++] = (Position){ant->position.x + dx, ant->position.y + dy};
      if (world_in_bounds(w, adj[ai - 1])) {
        CellType t = w->grid[adj[ai - 1].y * w->width + adj[ai - 1].x].type;
        if (t != CELL_EMPTY) {
          all_empty = false;
        }
      }
    }
  }
  if (all_empty) {
    Action move_down = {
        .type = ACTION_MOVE,
        .target = {ant->position.x, ant->position.y + 1},
        .resource = {0},
    };
    ant->plan[0] = move_down;
    ant->plan_length = 1;
    return;
  }

  int added = 0;
  while (added < PLAN_SIZE) {
    Action a;
    a.type = ACTION_IDLE;
    a.target = ant->position;
    a.resource = (Resource){0};

    int choice = rand() % 4; // 0:IDLE,1:MOVE,2:PICKUP,3:DROP

    if (choice == 1) { // MOVE
      Position candidates[8];
      int cand = 0;
      for (int k = 0; k < 8; k++) {
        Position t = adj[k];
        if (!world_in_bounds(w, t))
          continue;
        CellType tt = w->grid[t.y * w->width + t.x].type;
        /* allow move onto empty, nest or other ants (can climb on ants)
           but not onto resources */
        if (tt == CELL_RESOURCE)
          continue;

        /* enforce: the destination must be adjacent to at least one non-empty
         * cell */
        bool near_non_empty = false;
        for (int dy = -1; dy <= 1 && !near_non_empty; dy++) {
          for (int dx = -1; dx <= 1 && !near_non_empty; dx++) {
            if (dx == 0 && dy == 0)
              continue;
            Position n = {t.x + dx, t.y + dy};
            if (!world_in_bounds(w, n))
              continue;
            CellType nt = w->grid[n.y * w->width + n.x].type;
            if (nt != CELL_EMPTY) {
              near_non_empty = true;
            }
          }
        }
        if (near_non_empty) {
          candidates[cand++] = t;
        }
      }
      if (cand > 0) {
        a.type = ACTION_MOVE;
        a.target = candidates[rand() % cand];
      } else {
        a.type = ACTION_IDLE;
      }

    } else if (choice == 2) { // PICKUP (only adjacent)
      if (ant->is_carring) {
        a.type = ACTION_IDLE;
      } else {
        Position candidates[8];
        int cand = 0;
        for (int k = 0; k < 8; k++) {
          Position t = adj[k];
          if (!world_in_bounds(w, t))
            continue;
          Cell *c = &w->grid[t.y * w->width + t.x];
          if (c->type == CELL_RESOURCE) {
            candidates[cand++] = t;
          }
        }
        if (cand > 0) {
          a.type = ACTION_PICKUP;
          a.target = candidates[rand() % cand];
        } else {
          a.type = ACTION_IDLE;
        }
      }

    } else if (choice == 3) { // DROP (only adjacent)
      if (!ant->is_carring) {
        a.type = ACTION_IDLE;
      } else {
        Position candidates[8];
        int cand = 0;
        for (int k = 0; k < 8; k++) {
          Position t = adj[k];
          if (!world_in_bounds(w, t))
            continue;
          Cell *c = &w->grid[t.y * w->width + t.x];
          if (c->type == CELL_EMPTY) {
            candidates[cand++] = t;
          }
        }
        if (cand > 0) {
          a.type = ACTION_DROP;
          a.target = candidates[rand() % cand];
          a.resource = ant->carried_resource;
        } else {
          a.type = ACTION_IDLE;
        }
      }
    }

    ant->plan[added++] = a;
    ant->plan_length = added;
  }
}

// Simulation
void system_update_ant(int ant_id, World *world) {
  if (!world || ant_id < 0 || ant_id >= world->num_ants) {
    return;
  }

  Ant *ant = &(world->ants[ant_id]);

  Action ant_action;
  if (!ant_next_action(ant, &ant_action)) {
    ant_think(ant_id, world);
    return;
  }

  switch (ant_action.type) {
  case ACTION_IDLE:
    break;

  case ACTION_MOVE: {
    Position target = ant_action.target;
    if (!world_in_bounds(world, target)) {
      ant_clear_plan(ant);
      return;
    }
    Cell *tc = &world->grid[target.y * world->width + target.x];
    /* disallow moving onto resources; allow empty, nest or other ants (climb)
     */
    if (tc->type == CELL_RESOURCE) {
      ant_clear_plan(ant);
      return;
    }

    if (world_in_bounds(world, ant->position)) {
      world_vacate_cell(world, ant->position);
    }

    Cell new_cell;
    new_cell.type = CELL_ANT;
    new_cell.ant_id = (ant_id >= 0) ? ant_id : 0;
    new_cell.pheromone_to_food = 0.0f;
    new_cell.pheromone_to_home = 0.0f;
    new_cell.pheromone_visited = 0.0f;
    new_cell.resource = (Resource){0};
    world_occupy_cell(world, target, new_cell);

    ant->position = target;
    break;
  }

  case ACTION_PICKUP: {
    Position target = ant_action.target;
    if (!world_in_bounds(world, target) || ant->is_carring) {
      ant_clear_plan(ant);
      return;
    }

    Cell *c = &world->grid[target.y * world->width + target.x];
    if (c->type != CELL_RESOURCE) {
      ant_clear_plan(ant);
      return;
    }

    /* pick up the resource */
    ant->is_carring = true;
    ant->carried_resource = c->resource;

    /* remove resource from world */
    c->type = CELL_EMPTY;
    c->resource = (Resource){0};
    break;
  }

  case ACTION_DROP: {
    Position target = ant_action.target;
    if (!world_in_bounds(world, target) || !ant->is_carring) {
      ant_clear_plan(ant);
      return;
    }

    Cell *c = &world->grid[target.y * world->width + target.x];
    if (c->type != CELL_EMPTY) {
      ant_clear_plan(ant);
      return;
    }

    Cell drop;
    drop.type = CELL_RESOURCE;
    drop.resource = ant->carried_resource;
    drop.pheromone_to_food = 0.0f;
    drop.pheromone_to_home = 0.0f;
    drop.pheromone_visited = 0.0f;
    drop.ant_id = 0;
    world_occupy_cell(world, target, drop);

    ant->is_carring = false;
    ant->carried_resource = (Resource){0};
    break;
  }
  }
}

void system_update_world(World *w) {
  if (!w)
    return;
  for (int i = 0; i < w->num_ants; i++) {
    system_update_ant(i, w);
  }
  
  /* Pheromone diffusion + evaporation + source injection */
  int width = w->width;
  int height = w->height;
  int n = width * height;

  float *new_food = calloc(n, sizeof(float));
  float *new_home = calloc(n, sizeof(float));
  float *new_visited = calloc(n, sizeof(float));
  if (!new_food || !new_home || !new_visited) {
    free(new_food);
    free(new_home);
    free(new_visited);
    return;
  }

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int idx = y * width + x;
      Cell *c = &w->grid[idx];

      int neigh[8];
      int nc = 0;
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0)
            continue;
          int nx = x + dx;
          int ny = y + dy;
          if (nx < 0 || nx >= width || ny < 0 || ny >= height)
            continue;
          neigh[nc++] = ny * width + nx;
        }
      }

      float v_food = c->pheromone_to_food;
      float v_home = c->pheromone_to_home;
      float v_visited = c->pheromone_visited;

      float d_food = v_food * DIFFUSION_RATE;
      float d_home = v_home * DIFFUSION_RATE;
      float d_visited = v_visited * DIFFUSION_RATE;

      float keep_food = v_food - d_food;
      float keep_home = v_home - d_home;
      float keep_visited = v_visited - d_visited;

      new_food[idx] += keep_food;
      new_home[idx] += keep_home;
      new_visited[idx] += keep_visited;

      if (nc > 0) {
        float share_food = d_food / nc;
        float share_home = d_home / nc;
        float share_visited = d_visited / nc;
        for (int k = 0; k < nc; k++) {
          int ni = neigh[k];
          new_food[ni] += share_food;
          new_home[ni] += share_home;
          new_visited[ni] += share_visited;
        }
      } else {
        new_food[idx] += d_food;
        new_home[idx] += d_home;
        new_visited[idx] += d_visited;
      }
    }
  }

  /* evaporation */
  for (int i = 0; i < n; i++) {
    new_food[i] *= EVAPORATION_RATE;
    new_home[i] *= EVAPORATION_RATE;
    new_visited[i] *= EVAPORATION_RATE;
  }

  for (int i = 0; i < n; i++) {
    w->grid[i].pheromone_to_food = new_food[i];
    w->grid[i].pheromone_to_home = new_home[i];
    w->grid[i].pheromone_visited = new_visited[i];
  }

  if (world_in_bounds(w, w->nest_pos)) {
    int ni = w->nest_pos.y * width + w->nest_pos.x;
    w->grid[ni].pheromone_to_home = 1.0f;
  }
  for (int i = 0; i < n; i++) {
    if (w->grid[i].type == CELL_RESOURCE &&
        w->grid[i].resource.type == RESOURCE_FOOD) {
      w->grid[i].pheromone_to_food = 1.0f;
    }
  }

  free(new_food);
  free(new_home);
  free(new_visited);
}