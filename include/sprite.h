#ifndef SKY_SPRITE_H
#define SKY_SPRITE_H

#include <map.h>

#include "collections.h"
#include <data_types.h>
#include "game_common.h"
#include <ecs.h>

typedef struct Animation {
    Rectangle mask;
    Point origin;
    SDL_Texture* texture;
    AnimationSet* states;
    char* default_state;
    int width;
    int height;
    float speed;
    SpriteUpdateMode update_mode;
} Animation;

typedef struct Sprite {
    Point origin;
    SDL_Texture* texture;
    AnimationSet* states;
    RectList* frames;
    char* state;
    int width;
    int height;
    int depth;
    int frame;
    int next;
    float speed;
    float ticks;
    SpriteUpdateMode update_mode;
} Sprite;

#define SPRITE_STATE_UP "up"
#define SPRITE_STATE_LEFT "left"
#define SPRITE_STATE_DOWN "down"
#define SPRITE_STATE_RIGHT "right"

void sprite_init(Sprite* sprite, Animation* texture);
void sprite_set_state(Sprite* sprite, const char* state);
const char* sprite_state_from_direction(float direction);

void sprite_system_update(EcsComponentSystem* system, float delta, void* component);

#endif