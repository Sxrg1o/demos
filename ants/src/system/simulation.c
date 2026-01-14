#include "simulation.h"
#include "../entities/ant.h"
#include "ant_math.h"
#include "ant_world.h"
#include <math.h>
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

  Action next_action;
  next_action.type = ACTION_IDLE;
  next_action.target = ant->position;
  next_action.resource = (Resource){0};

  // State Machine / Hierarchical AI

  // Update state based on reality check
  if (!ant->is_carring) {
    if (ant->state == STATE_RETURNING || ant->state == STATE_CLEARING) {
      ant->state = STATE_SCOUTING;
    }
  } else {
    if (ant->carried_resource.type == RESOURCE_FOOD &&
        ant->state != STATE_RETURNING) {
      ant->state = STATE_RETURNING;
    }
    if (ant->carried_resource.type == RESOURCE_DIRT &&
        ant->state != STATE_CLEARING) {
      ant->state = STATE_CLEARING;
      ant->origin_pos = ant->position;
    }
  }

  switch (ant->state) {
  case STATE_SCOUTING: {
    float min_score = 1000000.0f;
    Position candidates[8];
    int cand_count = 0;
    bool smells_food = false;

    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (!world_in_bounds(w, p))
          continue;
        Cell *c = &w->grid[p.y * w->width + p.x];

        if (c->pheromone_to_food > 0.1f)
          smells_food = true;

        if (c->type != CELL_RESOURCE) {
          float score =
              (c->pheromone_visited * 5.0f) + (c->pheromone_to_home * 1.0f);
          score += ((rand() % 100) / 100.0f) * 0.1f;
          if (score < min_score) {
            min_score = score;
            cand_count = 0;
            candidates[cand_count++] = p;
          } else if (score == min_score) {
            candidates[cand_count++] = p;
          }
        }
      }
    }
    if (smells_food) {
      ant->state = STATE_HARVESTING;
    } else if (cand_count > 0) {
      next_action.type = ACTION_MOVE;
      next_action.target = candidates[rand() % cand_count];
    }
    break;
  }

  case STATE_HARVESTING: {
    float max_food = -1.0f;
    Position candidates[8];
    int cand_count = 0;
    Position dirt_blocker = {-1, -1};
    float dirt_phero = -1.0f;

    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (!world_in_bounds(w, p))
          continue;
        Cell *c = &w->grid[p.y * w->width + p.x];

        if (c->type == CELL_RESOURCE && c->resource.type == RESOURCE_FOOD) {
          next_action.type = ACTION_PICKUP;
          next_action.target = p;
          ant->state = STATE_RETURNING;
          ant->plan[0] = next_action;
          ant->plan_length = 1;
          return;
        }

        if (c->pheromone_to_food > max_food) {
          max_food = c->pheromone_to_food;
          cand_count = 0;
          if (c->type != CELL_RESOURCE)
            candidates[cand_count++] = p;
          else if (c->resource.type == RESOURCE_DIRT) {
            dirt_blocker = p;
            dirt_phero = c->pheromone_to_food;
          }
        } else if (c->pheromone_to_food == max_food) {
          if (c->type != CELL_RESOURCE)
            candidates[cand_count++] = p;
        }
      }
    }

    bool should_dig = false;
    if (dirt_blocker.x != -1) {
      if (cand_count == 0 || dirt_phero > max_food)
        should_dig = true;
    }

    if (should_dig) {
      next_action.type = ACTION_PICKUP;
      next_action.target = dirt_blocker;
      ant->state = STATE_CLEARING;
      ant->origin_pos = ant->position;
    } else if (cand_count > 0) {
      next_action.type = ACTION_MOVE;
      next_action.target = candidates[rand() % cand_count];
    } else {
      ant->state = STATE_SCOUTING;
    }
    break;
  }

  case STATE_RETURNING: {
    int d_nest = dist_sq(ant->position, w->nest_pos);
    if (d_nest <= 2) {
      next_action.type = ACTION_DROP;
      next_action.target = w->nest_pos;
      next_action.resource = ant->carried_resource;
      ant->state = STATE_SCOUTING;
      ant->plan[0] = next_action;
      ant->plan_length = 1;
      return;
    }
    if (d_nest < 6 * 6 && d_nest > 2 * 2) {
      Position drops[8];
      int drop_cnt = 0;
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0)
            continue;
          Position p = {ant->position.x + dx, ant->position.y + dy};
          if (world_in_bounds(w, p) &&
              w->grid[p.y * w->width + p.x].type == CELL_EMPTY) {
            drops[drop_cnt++] = p;
          }
        }
      }
      if (drop_cnt > 0) {
        next_action.type = ACTION_DROP;
        next_action.target = drops[rand() % drop_cnt];
        next_action.resource = ant->carried_resource;
        ant->state = STATE_SCOUTING;
        ant->plan[0] = next_action;
        ant->plan_length = 1;
        return;
      }
    }

    float max_home = -1.0f;
    Position candidates[8];
    int cand_count = 0;
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (!world_in_bounds(w, p))
          continue;
        Cell *c = &w->grid[p.y * w->width + p.x];
        if (c->type != CELL_RESOURCE) {
          if (c->pheromone_to_home > max_home) {
            max_home = c->pheromone_to_home;
            cand_count = 0;
            candidates[cand_count++] = p;
          } else if (c->pheromone_to_home == max_home) {
            candidates[cand_count++] = p;
          }
        }
      }
    }
    if (cand_count > 0) {
      next_action.type = ACTION_MOVE;
      next_action.target = candidates[rand() % cand_count];
    }
    break;
  }

  case STATE_CLEARING: {
    int d_origin = dist_sq(ant->position, ant->origin_pos);
    if (d_origin > 5 * 5) {
      float min_phero = 1000.0f;
      Position drops[8];
      int drop_cnt = 0;
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0)
            continue;
          Position p = {ant->position.x + dx, ant->position.y + dy};
          if (world_in_bounds(w, p) &&
              w->grid[p.y * w->width + p.x].type == CELL_EMPTY) {
            float score = w->grid[p.y * w->width + p.x].pheromone_to_food +
                          w->grid[p.y * w->width + p.x].pheromone_to_home;
            if (score < min_phero) {
              min_phero = score;
              drop_cnt = 0;
              drops[drop_cnt++] = p;
            } else if (score == min_phero)
              drops[drop_cnt++] = p;
          }
        }
      }
      if (drop_cnt > 0) {
        next_action.type = ACTION_DROP;
        next_action.target = drops[rand() % drop_cnt];
        next_action.resource = ant->carried_resource;
        ant->state = STATE_HARVESTING;
        ant->plan[0] = next_action;
        ant->plan_length = 1;
        return;
      }
    }

    float min_score = 100000.0f;
    Position candidates[8];
    int cand_count = 0;
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        Position p = {ant->position.x + dx, ant->position.y + dy};
        if (!world_in_bounds(w, p))
          continue;
        Cell *c = &w->grid[p.y * w->width + p.x];
        if (c->type != CELL_RESOURCE) {
          float score = c->pheromone_to_food + c->pheromone_to_home;
          if (score < min_score) {
            min_score = score;
            cand_count = 0;
            candidates[cand_count++] = p;
          } else if (score == min_score)
            candidates[cand_count++] = p;
        }
      }
    }
    if (cand_count > 0) {
      next_action.type = ACTION_MOVE;
      next_action.target = candidates[rand() % cand_count];
    }
    break;
  }
  }

  ant->plan[0] = next_action;
  ant->plan_length = 1;
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
      // If leaving nest, restore nest type
      if (ant->position.x == world->nest_pos.x &&
          ant->position.y == world->nest_pos.y) {
        Cell *curr =
            &world->grid[ant->position.y * world->width + ant->position.x];
        curr->type = CELL_NEST;
        curr->ant_id = -1;
      } else {
        world_vacate_cell(world, ant->position);
      }
    }

    Cell new_cell;
    new_cell.type = CELL_ANT;
    new_cell.ant_id = (ant_id >= 0) ? ant_id : 0;

    // Preserve existing pheromones from target cell
    new_cell.pheromone_to_food = tc->pheromone_to_food;
    new_cell.pheromone_to_home = tc->pheromone_to_home;
    new_cell.pheromone_visited = tc->pheromone_visited;

    // Deposit trails
    if (ant->is_carring && ant->carried_resource.type == RESOURCE_FOOD) {
      if (new_cell.pheromone_to_food < 1.0f)
        new_cell.pheromone_to_food = 1.0f;
    }

    // Deposit exploration trail
    if (new_cell.pheromone_visited < 1.0f)
      new_cell.pheromone_visited = 1.0f;

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

    /* remove resource from world, preserving pheromones */
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

    // Drop logic
    Cell *c = &world->grid[target.y * world->width + target.x];

    bool is_nest =
        (target.x == world->nest_pos.x && target.y == world->nest_pos.y);
    if (c->type == CELL_NEST || is_nest) {
      // Consume
      ant->is_carring = false;
      ant->carried_resource = (Resource){0};
      return;
    }

    if (c->type != CELL_EMPTY) {
      ant_clear_plan(ant);
      return;
    }

    Cell drop;
    drop.type = CELL_RESOURCE;
    drop.resource = ant->carried_resource;
    // If this is food dropped near the nest, convert it to a reservoir.
    // The radius grows slowly with existing reservoir mass so deposits
    // start near the nest edge and expand as the reserve grows.
    if (drop.resource.type == RESOURCE_FOOD) {
      int total_mass = 0;
      int area = world->width * world->height;
      for (int ii = 0; ii < area; ii++) {
        Cell *rc = &world->grid[ii];
        if (rc->type == CELL_RESOURCE &&
            rc->resource.type == RESOURCE_RESERVOIR) {
          total_mass += rc->resource.value;
        }
      }

      // radius = 2 + sqrt(total_mass / 20). Keeps start near 2 and grows slowly
      int reservoir_radius = 2 + (int)sqrtf((float)total_mass / 20.0f);
      int maxr =
          (world->width < world->height ? world->width : world->height) / 2 - 1;
      if (reservoir_radius < 2)
        reservoir_radius = 2;
      if (reservoir_radius > maxr)
        reservoir_radius = maxr;

      if (dist_sq(target, world->nest_pos) <=
          reservoir_radius * reservoir_radius) {
        drop.resource.type = RESOURCE_RESERVOIR;
      }
    }
    // Preserve pheromones
    drop.pheromone_to_food = c->pheromone_to_food;
    drop.pheromone_to_home = c->pheromone_to_home;
    drop.pheromone_visited = c->pheromone_visited;
    drop.ant_id = -1;
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