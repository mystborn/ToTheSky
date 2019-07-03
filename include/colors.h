#ifndef SKY_COLORS_H
#define SKY_COLORS_H

#include <SDL.h>

extern SDL_PixelFormat* game_pixel_format;

extern Uint32 c_black;
extern Uint32 c_white;

#define color_to_uint(value) SDL_MapRGBA(game_pixel_format, value.r, value.g, value.b, value.a)

static inline SDL_Color uint_to_color(Uint32 value) {
    Uint8 r, g, b, a;
    SDL_GetRGBA(value, game_pixel_format, &r, &g, &b, &a);
    return (SDL_Color) { .r = r, .g = g, .b = b, .a = a };
}

void colors_init(Uint32 pixel_format);

#endif