#include "torch.h"

#include <tickit.h>

static void __draw_map(TickitRenderBuffer *rb, TickitPen *pen, struct tile (*map)[MAP_LINES][MAP_COLS])
{
	const int viewy = player.posy - VIEW_LINES / 2;
	const int viewx = player.posx - VIEW_COLS / 2;

	for (int line = 0; line < VIEW_LINES; ++line) {
		for (int col = 0; col < VIEW_COLS; ++col) {
			struct tile tile = floor_map_at(cur_floor, viewy + line, viewx + col);
			whatdoicallthis(rb, pen, tile.r * tile.light + tile.dr, tile.g * tile.light + tile.dg, tile.b * tile.light + tile.db);
			tickit_renderbuffer_text_at(rb, line, col, (char[]){tile.token, '\0'});
		}
	}
}

void draw_map(TickitRenderBuffer *rb, TickitPen *pen)
{
	__draw_map(rb, pen, &cur_floor->map);
}

static void __draw_entities(TickitRenderBuffer *rb, TickitPen *pen, entity_list *entities)
{
	struct entity *pos;
	list_for_each_entry(pos, entities, list) {
		int line = pos->posy - (player.posy - VIEW_LINES / 2);
		int col = pos->posx - (player.posx - VIEW_COLS / 2);
		if (line >= VIEW_LINES || col >= VIEW_COLS)
			continue;
		struct tile tile = floor_map_at(cur_floor, pos->posy, pos->posx);
		fprintf(debug_log, "dr: %d dg: %d db: %d", tile.dr, tile.dg, tile.db);
		whatdoicallthis(rb, pen, pos->r + tile.dr, pos->g + tile.dg, pos->b + tile.db);
		tickit_renderbuffer_text_at(rb, line, col, (char[]){ pos->token, '\0'});
	}
}

void draw_entities(TickitRenderBuffer *rb, TickitPen *pen)
{
	__draw_entities(rb, pen, &cur_floor->entities);
}

void whatdoicallthis(TickitRenderBuffer *rb, TickitPen *pen, uint8_t r, uint8_t g, uint8_t b)
{
	tickit_pen_set_colour_attr(pen, TICKIT_PEN_FG, 1);
	TickitPenRGB8 rgb = { .r = r, .g = g, .b = b };
	tickit_pen_set_colour_attr_rgb8(pen, TICKIT_PEN_FG, rgb);
	tickit_renderbuffer_setpen(rb, pen);
}
