#include <render_system.h>

#include <common_events.h>
#include <limits.h>
#include <game.h>

static void render_system_free(void* data, EcsSystem* system) {
    RenderSystem* draw = (RenderSystem*)system;
    ecs_entity_set_free(draw->set);
    entity_list_free(draw->current);
    entity_list_free(draw->standby);
    free(draw->current);
    free(draw->standby);
}

static void draw_entity(Rectangle* camera_bounds, Entity entity, Sprite* sprite) {
    Vector2* position;
    ecs_component_get(entity, position_component, &position);

    Rectangle sprite_bounds = 
    { 
        (int)(position->x - sprite->origin.x),
        (int)(position->y - sprite->origin.x),
        sprite->width,
        sprite->height  
    };

    if(!SDL_HasIntersection(camera_bounds, &sprite_bounds))
        return;

    SDL_RenderCopyEx(renderer, sprite->texture, sprite->frames->buffer + sprite->frame, &sprite_bounds, 0.0f, &sprite->origin, SDL_FLIP_NONE);
}

static void render_system_update(EcsActionSystem* system, float delta) {
    RenderSystem* draw = (RenderSystem*)system;

    int count;
    Entity* entities = ecs_entity_set_get_entities(draw->set, &count);

    Rectangle camera_bounds = camera_get_bounds(draw->camera);

    int minimum = INT_MAX;
    Sprite* sprite;
    for(int i = 0; i < count; i++) {
        ecs_component_get(entities[i], sprite_component, &sprite);
        if(sprite->depth < minimum)
            minimum = sprite->depth;
        entity_list_add(draw->current, entities[i]);
    }

    while(entity_list_count(draw->current) != 0) {
        int current = minimum;
        minimum = INT_MAX;

        for(int i = 0; i < draw->current->count; i++) {
            ecs_component_get(entity_list_get(draw->current, i), sprite_component, &sprite);
            if(sprite->depth > current) {
                entity_list_add(draw->standby, entity_list_get(draw->current, i));
                if(sprite->depth < minimum)
                    minimum = sprite->depth;
                continue;
            }
            draw_entity(&camera_bounds, entity_list_get(draw->current, i), sprite);
        }
        entity_list_clear(draw->current);

        EntityList* temp = draw->standby;
        draw->standby = draw->current;
        draw->current = temp;
    }
}

void render_system_init(RenderSystem* system, 
                      EcsWorld world,
                      Camera* camera) 
{
    ecs_action_system_init(&system->base, render_system_update, NULL, NULL);
    system->camera = camera;
    EntitySetBuilder* builder = ecs_entity_set_builder_init();
    ecs_entity_set_with(builder, sprite_component);
    ecs_entity_set_with(builder, position_component);
    system->set = ecs_entity_set_build(builder, world, true);
    ecs_event_add(ecs_system_get_dispose_event(system), ecs_closure(NULL, render_system_free));
    system->current = entity_list_create();
    system->standby = entity_list_create();
}