#include <spatial_hash.h>
#include <common_events.h>
#include <math_utils.h>

static inline uint32_t long_hash(Sint64 value) {
    return ((int)value) ^ ((int)(value >> 32));
}

static inline int long_cmp(Sint64 left, Sint64 right) {
    return left < right ? -1 : (left > right ? 1 : 0);
}

LIST_DEFINE_C(ColliderList, collider_list, Collider)

MAP_DEFINE_C(SpatialMap, spatial_map, Sint64, ColliderList*, long_hash, long_cmp, NULL)


#define SPATIAL_HASH_CELL_SIZE 128
#define SPATIAL_HASH_INVERSE_SIZE (1.0f / 128)

static inline Sint64 spatial_hash_get_key(int x, int y) {
    return ((Sint64)x << 32) | (Uint32)y;
}

static inline ColliderList* spatial_hash_get_cell(SpatialHash* hash, Sint64 key, bool create_new_cell) {
    ColliderList* cell = spatial_map_get(hash->map, key);
    if(cell == NULL && create_new_cell) {
        cell = malloc(sizeof(*cell));
        collider_list_init(cell);
        spatial_map_set(hash->map, key, cell);
    }

    return cell;
}

static void spatial_hash_component_added(void* data, EcsComponentAddedMessage* message) {
    if(message->component_type == position_component) {
        if(!ecs_component_exists(message->entity, collider_component))
            return;
    } else {
        if(!ecs_component_exists(message->entity, position_component))
            return;
    }

    SpatialHash* hash = data;

    // If the entity has both components, add it to the hash.
    Vector2* position;
    Vector2* size;

    ecs_component_get(message->entity, position_component, &position);
    ecs_component_get(message->entity, collider_component, &size);

    int x1 = fast_floor(position->x * SPATIAL_HASH_INVERSE_SIZE);
    int y1 = fast_floor(position->y * SPATIAL_HASH_INVERSE_SIZE);
    int x2 = fast_floor((position->x + size->x) * SPATIAL_HASH_INVERSE_SIZE);
    int y2 = fast_floor((position->y + size->y) * SPATIAL_HASH_INVERSE_SIZE);

    Collider collider = { .owner = message->entity, .bounds = { position->x, position->y, size->x, size->y } };

    for(int x = x1; x <= x2; x++) {
        for(int y = y1; y <= y2; y++) {
            long key = spatial_hash_get_key(x, y);
            ColliderList* cell = spatial_hash_get_cell(hash, key, true);
            collider_list_add(cell, collider);
        }
    }
}

static void spatial_hash_component_removed(void* data, EcsComponentRemovedMessage* message) {
    // If the entity only had one component to begin with, we can just exit early.
    if(message->component_type == position_component) {
        if(!ecs_component_exists(message->entity, collider_component))
            return;
    } else {
        if(!ecs_component_exists(message->entity, position_component))
            return;
    }

    // Otherwise, remove the collider from all lists containing it.
    SpatialHash* hash = data;

    Vector2* position;
    Vector2* size;

    ecs_component_get(message->entity, position_component, &position);
    ecs_component_get(message->entity, collider_component, &size);

    int x1 = fast_floor(position->x * SPATIAL_HASH_INVERSE_SIZE);
    int y1 = fast_floor(position->y * SPATIAL_HASH_INVERSE_SIZE);
    int x2 = fast_floor((position->x + size->x) * SPATIAL_HASH_INVERSE_SIZE);
    int y2 = fast_floor((position->y + size->y) * SPATIAL_HASH_INVERSE_SIZE);

    for(int x = x1; x <= x2; x++) {
        for(int y = y1; y <= y2; y++) {
            long key = spatial_hash_get_key(x, y);
            ColliderList* cell = spatial_hash_get_cell(hash, key, true);
            for(int i = 0; i < cell->count; i++) {
                if(cell->buffer[i].owner.id == message->entity.id) {
                    collider_list_remove(cell, i);
                    break;
                }
            }
        }
    }
}

SpatialHash* spatial_hash_init(EcsWorld world) {
    SpatialHash* result = malloc(sizeof(SpatialHash));
    result->map = malloc(sizeof(*result->map));
    result->cache = malloc(sizeof(*result->cache));
    spatial_map_init(result->map);
    collider_cache_init(result->cache);
    result->world = world;
    result->subscriptions[0] = ecs_event_subscribe(world, ecs_component_get_added_event(position_component), ecs_closure(result, spatial_hash_component_added));
    result->subscriptions[1] = ecs_event_subscribe(world, ecs_component_get_added_event(collider_component), ecs_closure(result, spatial_hash_component_added));
    result->subscriptions[2] = ecs_event_subscribe(world, ecs_component_get_removed_event(position_component), ecs_closure(result, spatial_hash_component_removed));
    result->subscriptions[3] = ecs_event_subscribe(world, ecs_component_get_removed_event(collider_component), ecs_closure(result, spatial_hash_component_removed));
    return result;
}

void spatial_hash_free(SpatialHash* hash) {
    spatial_map_free(hash->map);
    collider_cache_free_resources(hash->cache);
    free(hash->map);
    free(hash->cache);
    ecs_event_unsubscribe(hash->world, ecs_component_get_added_event(position_component), hash->subscriptions[0]);
    ecs_event_unsubscribe(hash->world, ecs_component_get_added_event(collider_component), hash->subscriptions[1]);
    ecs_event_unsubscribe(hash->world, ecs_component_get_removed_event(position_component), hash->subscriptions[2]);
    ecs_event_unsubscribe(hash->world, ecs_component_get_removed_event(collider_component), hash->subscriptions[3]);
}

void spatial_hash_move(SpatialHash* hash, Entity entity, Vector2 old_position) {
    Vector2* position;
    Vector2* size;

    ecs_component_get(entity, position_component, &position);
    ecs_component_get(entity, collider_component, &size);

    int x1 = fast_floor(old_position.x * SPATIAL_HASH_INVERSE_SIZE);
    int y1 = fast_floor(old_position.y * SPATIAL_HASH_INVERSE_SIZE);
    int x2 = fast_floor((old_position.x + size->x) * SPATIAL_HASH_INVERSE_SIZE);
    int y2 = fast_floor((old_position.y + size->y) * SPATIAL_HASH_INVERSE_SIZE);

    for(int x = x1; x <= x2; x++) {
        for(int y = y1; y <= y2; y++) {
            long key = spatial_hash_get_key(x, y);
            ColliderList* cell = spatial_hash_get_cell(hash, key, true);
            for(int i = 0; i < cell->count; i++) {
                if(cell->buffer[i].owner.id == entity.id) {
                    collider_list_remove(cell, i);
                    break;
                }
            }
        }
    }

    x1 = fast_floor(position->x * SPATIAL_HASH_INVERSE_SIZE);
    y1 = fast_floor(position->y * SPATIAL_HASH_INVERSE_SIZE);
    x2 = fast_floor((position->x + size->x) * SPATIAL_HASH_INVERSE_SIZE);
    y2 = fast_floor((position->y + size->y) * SPATIAL_HASH_INVERSE_SIZE);

    Collider collider = { .owner = entity, .bounds = { position->x, position->y, size->x, size->y } };

    for(int x = x1; x <= x2; x++) {
        for(int y = y1; y <= y2; y++) {
            long key = spatial_hash_get_key(x, y);
            ColliderList* cell = spatial_hash_get_cell(hash, key, true);
            collider_list_add(cell, collider);
        }
    }
}

const Collider* spatial_hash_get_point(SpatialHash* hash, Vector2 point, int* count) {
    int x = fast_floor(point.x * SPATIAL_HASH_INVERSE_SIZE);
    int y = fast_floor(point.y * SPATIAL_HASH_INVERSE_SIZE);

    ColliderList* cell = spatial_map_get(hash->map, spatial_hash_get_key(x, y));
    if(cell == NULL) {
        *count = 0;
        return NULL;
    }

    collider_cache_reset(hash->cache);

    for(int i = 0; i < cell->count; i++)
        if(rectf_contains_vector(&cell->buffer[i].bounds, point))
            collider_cache_add(hash->cache, cell->buffer[i]);

    return collider_cache_get_colliders(hash->cache, count);
}

const Collider* spatial_hash_get_rect(SpatialHash* hash, RectangleF rect, int* count) {
    collider_cache_reset(hash->cache);

    int x1 = fast_floor(rect.x * SPATIAL_HASH_INVERSE_SIZE);
    int y1 = fast_floor(rect.y * SPATIAL_HASH_INVERSE_SIZE);
    int x2 = fast_floor((rect.x + rect.w) * SPATIAL_HASH_INVERSE_SIZE);
    int y2 = fast_floor((rect.y + rect.h) * SPATIAL_HASH_INVERSE_SIZE);

    for(int x = x1; x <= x2; x++) {
        for(int y = y1; y <= y2; y++) {
            long key = spatial_hash_get_key(x, y);
            ColliderList* cell = spatial_hash_get_cell(hash, key, false);
            if(cell == NULL)
                continue;

            for(int i = 0; i < cell->count; i++)
                if(rectf_intersects(&cell->buffer[i].bounds, &rect))
                    collider_cache_add(hash->cache, cell->buffer[i]);
        }
    }

    return collider_cache_get_colliders(hash->cache, count);
}

void collider_cache_init(ColliderCache* cache) {
    cache->mapping = NULL;
    cache->colliders = NULL;
    cache->mapping_count = 0;
    cache->collider_count = 0;
}

void collider_cache_free_resources(ColliderCache* cache) {
    free(cache->mapping);
    free(cache->colliders);
}

void collider_cache_add(ColliderCache* cache, Collider collider) {
    ECS_ARRAY_RESIZE_DEFAULT(cache->mapping, cache->mapping_count, collider.owner.id, sizeof(*cache->mapping), 0);

    Uint8* added = cache->mapping + collider.owner.id;
    if(*added != 0)
        return;

    *added = 1;

    int new_size = cache->collider_count + 1;
    ECS_ARRAY_RESIZE(cache->colliders, cache->collider_count, new_size, sizeof(*cache->colliders));

    cache->colliders[collider.owner.id] = collider;
}

void collider_cache_reset(ColliderCache* cache) {
    memset(cache->mapping, 0, cache->mapping_count * sizeof(*cache->mapping));
    cache->collider_count = 0;
}

Collider* collider_cache_get_colliders(ColliderCache* cache, int* count) {
    *count = cache->collider_count;
    return cache->colliders;
}