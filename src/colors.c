#include <colors.h>

SDL_PixelFormat* game_pixel_format;

Uint32 c_black;
Uint32 c_white;

void colors_init(Uint32 pixel_format) {
	game_pixel_format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
	c_black = SDL_MapRGB(game_pixel_format, 0, 0, 0);
	c_white = SDL_MapRGB(game_pixel_format, 255, 255, 255);
}