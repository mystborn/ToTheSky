#include <systems/player_system.h>
#include <common_events.h>
#include <sprite.h>
#include <input.h>

static bool player_set_direction(PlayerMovement* movement) {
    float aim = 361;
    float x = gamepad_axis_value(SDL_CONTROLLER_AXIS_LEFTX);
    float y = gamepad_axis_value(SDL_CONTROLLER_AXIS_LEFTY);
    if(x != 0 || y != 0) {
        aim = SDL_atan2f(-x, -y);
        printf("%g\n", aim);
        return true;
    }
}

static void player_system_update(EcsEntitySystem* system, float delta, Entity entity) {
    Sprite* sprite;
    Vector2* velocity;
    PlayerMovement* movement;

    ecs_component_get(entity, velocity_component, &velocity);
    ecs_component_get(entity, sprite_component, &sprite);
    ecs_component_get(entity, player_movement_component, &movement);


}

void player_system_init(PlayerSystem* system, EcsWorld world, SpatialHash* hash);