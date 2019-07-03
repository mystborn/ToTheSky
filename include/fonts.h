#ifndef SKY_FONTS_H
#define SKY_FONTS_H

#include <SDL_FontCache.h>
#include <stdbool.h>

extern FC_Font* f_text;

bool fonts_init(void);
void fonts_free(void);

#endif