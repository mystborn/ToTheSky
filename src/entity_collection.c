#include <entity_collection.h>
#include <stddef.h>
#include <stdlib.h>

void entity_collection_init(EntityCollection* list) {
    list->mapping = NULL;
    list->entities = NULL;
    list->mapping_count = 0;
    list->entity_count = 0;
}

void entity_collection_free_resources(EntityCollection* list) {
    free(list->mapping);
    free(list->entities);
}

void entity_collection_add(EntityCollection* list, Entity entity) {
    ECS_ARRAY_RESIZE(list->mapping, list->mapping_count, entity.id, sizeof(int));

    int* index = list->mapping + entity.id;
    if(*index != -1)
        return;

    *index = list->entity_count++;
    list->entities[*index] = entity;
}

void entity_collection_reset(EntityCollection* list) {
    memset(list->mapping, -1, list->mapping_count * sizeof(int));
    list->entity_count = 0;
}

Entity* entity_collection_get_entities(EntityCollection* list, int* count) {
    *count = list->entity_count;
    return list->entities;
}