#ifndef SKY_VIEWPORT_H
#define SKY_VIEWPORT_H

#include "data_types.h"

extern Rectangle virtual_viewport;
extern Rectangle viewport;

void viewport_init(SDL_Window* window, Rectangle virt_viewport);

#endif // !SKY_VIEWPORT_H
