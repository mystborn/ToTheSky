#ifndef SKY_ENTITY_LIST_H
#define SKY_ENTITY_LIST_H

#include <ecs.h>

typedef struct EntityCollection {
    int* mapping;
    Entity* entities;
    int mapping_count;
    int entity_count;
} EntityCollection;

void entity_collection_init(EntityCollection* list);
void entity_collection_free_resources(EntityCollection* list);

void entity_collection_add(EntityCollection* list, Entity entity);
void entity_collection_reset(EntityCollection* list);
Entity* entity_collection_get_entities(EntityCollection* list, int* count);


#endif