#include "torch.h"

#include <tickit.h>
#include <stdlib.h>
#include <stdio.h>

#define MAP_LINES 20
#define MAP_COLS  40

struct tile map[MAP_LINES][MAP_COLS];

#define VIEW_LINES 23
#define VIEW_COLS  79

struct entity player = {
	.posx = 0, .posy = 0
};

static void key_left(char key);
static void key_up(char key);
static void key_down(char key);
static void key_right(char key);

typedef void (*keymap_fn)(char key);
keymap_fn keymap[] = {
	['h'] = key_left,
	['j'] = key_down,
	['k'] = key_up,
	['l'] = key_right
};

static int root_handle_key(TickitWindow *win, TickitEventFlags flags, void *info, void *data)
{
	TickitKeyEventInfo *key = info;
	switch (key->type) {
	case TICKIT_KEYEV_TEXT: /* Pain. */
		if (keymap[*key->str])
			keymap[*key->str](*key->str);
	}

	tickit_window_expose(win, NULL);

	return 1;
}

static int root_handle_expose(TickitWindow *win, TickitEventFlags flags, void *info, void *data)
{
	TickitExposeEventInfo *exposed = info;
	TickitRenderBuffer *rb = exposed->rb;

	tickit_renderbuffer_eraserect(rb, &exposed->rect);

	const int viewy = player.posy - (VIEW_LINES / 2);
	const int viewx = player.posx - (VIEW_COLS / 2);

	for (int y = 0; y < VIEW_LINES; ++y) {
		for (int x = 0; x < VIEW_COLS; ++x) {
			const int drawy = viewy + y;
			const int drawx = viewx + x;

			if (in_range(drawy, 0, MAP_LINES) && in_range(drawx, 0, MAP_COLS))
				tickit_renderbuffer_text_at(rb, y, x, (char[]){map[drawy][drawx].sprite, '\0'});
			else
				tickit_renderbuffer_text_at(rb, y, x, " ");
		}
	}

	tickit_renderbuffer_text_at(rb, VIEW_LINES / 2 + 1, VIEW_COLS / 2 + 1, "@");

	return 1;
}

void load_map(const char *filename)
{
	FILE *mapfd = fopen(filename, "r");

	for (size_t line = 0; line < MAP_LINES; ++line) {
		fscanf(mapfd, "%s", map[line]);
	}

	fclose(mapfd);
}

int main(int argc, char *argv[])
{
	Tickit *tickit_instance = NULL;
	tickit_instance = tickit_new_stdio();

	TickitWindow *root = tickit_get_rootwin(tickit_instance);
	tickit_window_bind_event(root, TICKIT_WINDOW_ON_KEY, 0, &root_handle_key, NULL);
	tickit_window_bind_event(root, TICKIT_WINDOW_ON_EXPOSE, 0, &root_handle_expose, NULL);

	load_map("map");

	tickit_run(tickit_instance);

	tickit_window_close(root);

	tickit_unref(tickit_instance);

	return EXIT_SUCCESS;
}

static void key_left(char key)
{
	player.posx--;
}

static void key_up(char key)
{
	player.posy--;
}

static void key_down(char key)
{
	player.posy++;
}

static void key_right(char key)
{
	player.posx++;
}
