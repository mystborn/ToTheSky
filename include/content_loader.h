#ifndef SKY_CONTENT_LOADER_H
#define SKY_CONTENT_LOADER_H

#include "game_common.h"
#include "sprite.h"

void content_loader_init(void);

SDL_Texture* texture_load(char* path);
void texture_free(SDL_Texture* texture);
void texture_free_all(void);

Animation* animation_load(char* path);
void animation_free(Animation* animation);
void animation_free_all(void);

#endif