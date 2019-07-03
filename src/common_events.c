#include <common_events.h>
#include <sprite.h>
#include <systems/player_system.h>

EcsEvent* window_changed_event;

ComponentManager* sprite_component;
ComponentManager* position_component;
ComponentManager* collider_component;
ComponentManager* velocity_component;
ComponentManager* player_movement_component;
ComponentManager* gui_component;

static void velocity_constructor(void* component) {
    Vector2* velocity = component;
    velocity->x = 0;
    velocity->y = 0;
}

void common_events_init(void) {
	window_changed_event = ecs_event_init();
    sprite_component = ecs_component_define(sizeof(Sprite), NULL, NULL);
    position_component = ecs_component_define(sizeof(Vector2), NULL, NULL);
    collider_component = ecs_component_define(sizeof(Vector2), NULL, NULL);
    velocity_component = ecs_component_define(sizeof(Vector2), velocity_constructor, NULL);
    player_movement_component = ecs_component_define(sizeof(PlayerMovement), NULL, NULL);
    gui_component = ecs_component_define(0, NULL, NULL);
}
