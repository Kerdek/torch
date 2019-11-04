#ifndef TORCH
#define TORCH

#include "list.h"

#include <tickit.h>
#include <stdio.h>
#include <stdint.h>

#define back(arr) (arr[sizeof(arr)/sizeof(*arr)])
#define min(a, b) (a < b ? a : b)

typedef unsigned int uint;

extern FILE *debug_log;

#define def_main_win_key_fn(name) void name(void)
typedef def_main_win_key_fn(main_win_key_fn);

/* Entity */
struct entity;

#define def_entity_fn(name) void name(struct entity *this)
typedef def_entity_fn(entity_fn);

struct entity {
	uint8_t r, g, b;
	char token;
	int posy, posx;
	entity_fn *update;
	entity_fn *destroy;
	struct list_head list;
	struct floor *floor;
};

void entity_move_pos(struct entity *e, int y, int x);
void entity_move_pos_rel(struct entity *e, int y, int x);

/* Player */
extern struct entity player;

def_main_win_key_fn(player_move_left);
def_main_win_key_fn(player_move_down);
def_main_win_key_fn(player_move_up);
def_main_win_key_fn(player_move_right);

/* Floor */
struct tile {
	uint8_t r, g, b;
	char token;
	int light;
	int dr, dg, db;
	struct entity *entity;
	uint walk : 1;
};

#define MAP_LINES 100
#define MAP_COLS  100

typedef struct tile tile_map[MAP_LINES][MAP_COLS];
typedef struct list_head entity_list;

struct floor {
	tile_map map;
	entity_list entities;
};

extern struct floor *cur_floor;

enum floor_type {
	CAVE,
};

struct tile floor_map_at(struct floor *floor, int y, int x);
int         floor_map_in_bounds(int y, int x);
void        floor_map_clear_lights(void);
void        floor_map_generate(struct floor *floor, enum floor_type type);

void floor_add_entity(struct floor *floor, struct entity *entity);

void floor_update_entities(struct floor *floor);

#define floor_for_each_tile(pos, floor) \
	for (pos = *floor->map; pos != back(floor->map); ++pos)

/* Draw */
#define VIEW_LINES 23
#define VIEW_COLS  79

void draw_map(TickitRenderBuffer *rb, TickitPen *pen);
void draw_entities(TickitRenderBuffer *rb, TickitPen *pen);

/* Main Window */
TickitWindowEventFn main_win_on_key;
TickitWindowEventFn main_win_draw;

/* Demo */
void demo_floor_load_map(const char *filename);

def_entity_fn(demo_player_destroy);
def_entity_fn(demo_player_update);

void demo_add_entities(void);

extern struct floor demo_floor;

#endif
