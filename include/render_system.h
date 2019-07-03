#ifndef SKY_DRAW_SYSTEM_H
#define SKY_DRAW_SYSTEM_H

#include "collections.h"
#include "game_common.h"
#include <ecs.h>
#include "camera.h"
#include "sprite.h"

typedef struct RenderSystem {
    EcsActionSystem base;
    Camera* camera;
    EntitySet* set;
    EntityList* current;
    EntityList* standby;
} RenderSystem;

void render_system_init(RenderSystem* system, 
                        EcsWorld world,
                        Camera* camera);

#endif