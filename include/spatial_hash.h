#ifndef SKY_SPATIAL_HASH_H
#define SKY_SPATIAL_HASH_H

#include <list.h>
#include <map.h>

#include "game_common.h"
#include "collections.h"
#include "entity_collection.h"
#include "rectf.h"

typedef struct Collider {
    Entity owner;
    RectangleF bounds;
} Collider;

LIST_DEFINE_H(ColliderList, collider_list, Collider)

MAP_DEFINE_H(SpatialMap, spatial_map, Sint64, ColliderList*)

typedef struct ColliderCache {
    Uint8* mapping;
    Collider* colliders;
    int mapping_count;
    int collider_count;
} ColliderCache;

typedef struct SpatialHash {
    SpatialMap* map;
    ColliderCache* cache;
    EcsWorld world;
    int subscriptions[4];
} SpatialHash;

SpatialHash* spatial_hash_init(EcsWorld world);
void spatial_hash_free(SpatialHash* hash);
void spatial_hash_move(SpatialHash* hash, Entity entity, Vector2 old_position);
const Collider* spatial_hash_get_point(SpatialHash* hash, Vector2 point, int* count);
const Collider* spatial_hash_get_rect(SpatialHash* hash, RectangleF rect, int* count);

void collider_cache_init(ColliderCache* cache);
void collider_cache_free_resources(ColliderCache* cache);

void collider_cache_add(ColliderCache* cache, Collider collider);
void collider_cache_reset(ColliderCache* cache);
Collider* collider_cache_get_colliders(ColliderCache* cache, int* count);

#endif