#ifndef SKY_GAME_H
#define SKY_GAME_H

#include "game_common.h"
#include "timer.h"

typedef struct Game {
	SDL_Window* window;
	Uint32 background_color;
	SDL_DisplayMode display_mode;
} Game;

Game* game_init(char* title, int x, int y, int width, int height, Uint32 window_flags);
bool game_run(Game* game);
void game_destroy(Game* game);
void game_set_background(Game* game, Uint32 color);
Uint32 game_get_background(Game* game);

extern SDL_Renderer* renderer;

#endif // !SKY_GAME_H
