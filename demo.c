#include "torch.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

struct floor demo_floor;
struct floor *cur_floor = &demo_floor;

static void cast_light(tile_map *map, int radius, int y, int x, float bright, int r, int g, int b);
static void __cast_light(tile_map *map,int x, int y, int radius, int row,
	float start_slope, float end_slope, int xx, int xy, int yx, int yy,
	float bright, int r, int g, int b);


void demo_floor_load_map(const char *filename)
{
	FILE *mapfd = fopen(filename, "r");
	floor_map_generate(&demo_floor, CAVE);

	/* Make this not shit please. */
	for (size_t line = 0; line < MAP_LINES; ++line) {
		for (size_t col = 0; col < MAP_COLS; ++col) {
//			fscanf(mapfd, "%c", &demo_floor.map[line][col].token);
			demo_floor.map[line][col].light = 0;
			demo_floor.map[line][col].r = 51;
			demo_floor.map[line][col].g = 51;
			demo_floor.map[line][col].b = 51;

			if (demo_floor.map[line][col].token != '.') {
				demo_floor.map[line][col].g = 150;
				demo_floor.map[line][col].b = 150;
			}
		}
		(void)fgetc(mapfd);
	}

	fclose(mapfd);
}

def_entity_fn(demo_player_update)
{
	int y = this->posy;
	int x = this->posx;
	struct tile (*map)[MAP_LINES][MAP_COLS] = &cur_floor->map;
	cast_light(map, 5, y, x, 0.3f, this->r, this->g, this->b);
}

def_entity_fn(demo_torch_update)
{
	int y = (rand() % 3 - 1);
	int x = (rand() % 3 - 1);

	entity_move_pos_rel(this, y, x);

	y = this->posy;
	x = this->posx;
	struct tile (*map)[MAP_LINES][MAP_COLS] = &cur_floor->map;
	cast_light(map, 5, y, x, 0.7f, this->r, this->g, this->b);
}

def_entity_fn(demo_torch_destroy)
{

}

void demo_add_entities(void)
{
	struct entity torch = {
		.r = 0xe2, .g = /*0x58*/0, .b = 0x22,
		.token = 't',
		.posy = 12, .posx = 7,
		.update = demo_torch_update,
		.destroy = demo_torch_destroy,
		.list = LIST_HEAD_INIT(torch.list),
		.floor = cur_floor,
	};

	struct entity *t1 = malloc(sizeof(torch));
	struct entity *t2 = malloc(sizeof(torch));

	memcpy(t1, &torch, sizeof(torch));
	torch.r = 0x5e;
	torch.g = /*0xba*/0;
	torch.b = 0xc9;
	torch.posy = 12;
	torch.posx = 8;
	memcpy(t2, &torch, sizeof(torch));

	floor_add_entity(cur_floor, t1);
	floor_add_entity(cur_floor, t2);

	struct entity rock = {
		.r = 170, .g = 170, .b = 170,
		.token = '?',
		.posy = 13, 13,
		.update = NULL,
		.destroy = NULL,
		.list = LIST_HEAD_INIT(torch.list),
		.floor = cur_floor,
	};

	struct entity *r = malloc(sizeof(rock));
	memcpy(r, &rock, sizeof(rock));
	floor_add_entity(cur_floor, r);

}

#include <stdint.h>
#include <stdlib.h>

struct entity demo_new_torch(int y, int x)
{
	uint8_t r = rand() % 256;
	uint8_t g = rand() % 256;
	uint8_t b = rand() % 256;

	struct entity torch = {
		//.r = 0xe2, .g = 0, .b = 0x22,
		.r = r, .g = g, .b = b,
		.token = 't',
		.posy = y, .posx = x,
		.update = demo_torch_update,
		.destroy = demo_torch_destroy,
		.list = LIST_HEAD_INIT(torch.list),
		.floor = cur_floor,
	};

	return torch;
}

def_main_win_key_fn(place_torch)
{
	struct entity torch = demo_new_torch(player.posy, player.posx);
	struct entity *t = malloc(sizeof(torch));
	memcpy(t, &torch, sizeof(torch));
	floor_add_entity(cur_floor, t);
}

int drawn_to[MAP_LINES][MAP_COLS] = { 0 };

static void cast_light(tile_map *map, int radius, int y, int x, float bright, int r, int g, int b)
{
	__cast_light(map, x, y, radius, 1, 1.0, 0.0, 1, 0, 0, 1, bright, r, g, b);
	__cast_light(map, x, y, radius, 1, 1.0, 0.0, 0, 1, 1, 0, bright, r, g, b);
	__cast_light(map, x, y, radius, 1, 1.0, 0.0, 0, -1, 1, 0, bright, r, g, b);
	__cast_light(map, x, y, radius, 1, 1.0, 0.0, -1, 0, 0, 1, bright, r, g, b);
	__cast_light(map, x, y, radius, 1, 1.0, 0.0, -1, 0, 0, -1, bright, r, g, b);
	__cast_light(map, x, y, radius, 1, 1.0, 0.0, 0, -1, -1, 0, bright, r, g, b);
	__cast_light(map, x, y, radius, 1, 1.0, 0.0, 0, 1, -1, 0, bright, r, g, b);
	__cast_light(map, x, y, radius, 1, 1.0, 0.0, 1, 0, 0, -1, bright, r, g, b);

	struct tile tile = (*map)[y][x];
	(*map)[y][x].light = bright + tile.light;
	(*map)[y][x].dr = min(r * bright + tile.dr, 255);
	(*map)[y][x].dg = min(g * bright + tile.dg, 255);
	(*map)[y][x].db = min(b * bright + tile.db, 255);

	memset(drawn_to, 0, (sizeof(drawn_to[0][0]) * MAP_LINES * MAP_COLS));
}

static void __cast_light(tile_map *map, int x, int y, int radius, int row,
	float start_slope, float end_slope, int xx, int xy, int yx, int yy,
	float bright, int r, int g, int b)
{
	if (start_slope < end_slope) {
		return;
	}

	float next_start_slope = start_slope;
	for (int i = row; i <= radius; ++i) {
		int blocked = 0;
		for (int dx = -i, dy = -i; dx <= 0; dx++) {
			float l_slope = (dx - 0.5) / (dy + 0.5);
			float r_slope = (dx + 0.5) / (dy - 0.5);
			if (start_slope < r_slope) {
				continue;
			} else if (end_slope > l_slope) {
				break;
			}

			int sax = dx * xx + dy * xy;
			int say = dx * yx + dy * yy;
			if ((sax < 0 && abs(sax) > x) ||
				(say < 0 && abs(say) > y)) {
				continue;
			}
			uint ax = x + sax;
			uint ay = y + say;
			if (!floor_map_in_bounds(ay, ax)) {
				continue;
			}

			uint radius2 = radius * radius;
			if ((uint)(dx * dx + dy * dy) < radius2 && !drawn_to[ay][ax]) {
				drawn_to[ay][ax] = 1;
				int distance = sqrt(dx * dx + dy * dy);
				const float dlight = bright / (distance + 1);
				struct tile tile = (*map)[ay][ax];
				(*map)[ay][ax].light = dlight + tile.light;
				(*map)[ay][ax].dr = min(r * dlight + tile.dr, 255);
				(*map)[ay][ax].dg = min(g * dlight + tile.dg, 255);
				(*map)[ay][ax].db = min(b * dlight + tile.db, 255);
			}

			struct tile tile = (*map)[ay][ax];
			if (blocked) {
				if (!tile.walk || tile.entity) {
					next_start_slope = r_slope;
					continue;
				} else {
					blocked = 0;
					start_slope = next_start_slope;
				}
			} else if (!tile.walk || tile.entity) {
				blocked = 1;
				next_start_slope = r_slope;
				__cast_light(map, x, y, radius, i + 1,
					start_slope, l_slope, xx, xy, yx, yy,
					bright, r, g, b);
			}
		}
		if (blocked) {
			break;
		}
	}
}
