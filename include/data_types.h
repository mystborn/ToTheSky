#ifndef SKY_DATA_TYPES_H
#define SKY_DATA_TYPES_H

#include <SDL.h>

typedef struct Vector2 {
    float x;
    float y;
} Vector2;

typedef enum {
    SPRITE_UPDATE_CYCLE,
    SPRITE_UPDATE_PING_PONG
} SpriteUpdateMode;

typedef SDL_Point Point;
typedef SDL_Rect Rectangle;
typedef SDL_Texture Texture;

#endif //SKY_DATA_TYPES_H