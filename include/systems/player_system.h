#ifndef SKY_SYSTEMS_PLAYER_SYSTEM_H
#define SKY_SYSTEMS_PLAYER_SYSTEM_H

#include <game_common.h>
#include <ecs.h>
#include <spatial_hash.h>

typedef struct PlayerMovement {
    float move_dir;
} PlayerMovement;

typedef struct PlayerSystem {
    EcsEntitySystem base;
    SpatialHash* hash;
} PlayerSystem;

void player_system_init(PlayerSystem* system, EcsWorld world, SpatialHash* hash);

#endif