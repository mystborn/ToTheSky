#include <sprite.h>
#include <math_utils.h>

void sprite_init(Sprite* sprite, Animation* animation) {
    sprite->origin = animation->origin;
    sprite->texture = animation->texture;
    sprite->states = animation->states;
    sprite->state = animation->default_state;
    sprite->frames = animation_set_get(sprite->states, sprite->state);
    sprite->width = animation->width;
    sprite->height = animation->height;
    sprite->depth = 0;
    sprite->speed = animation->speed;
    sprite->ticks = 0;
    sprite->next = 1;
    sprite->update_mode = animation->update_mode;
}

void sprite_set_state(Sprite* sprite, const char* state) {
    if(strcmp(sprite->state, state) == 0)
        return;

    RectList* frames;
    if(!(frames = animation_set_get(sprite->states, (char*)state))) {
        fprintf(stderr, "Invalid sprite state: %s", state);
        return;
    }

    sprite->state = state;
    sprite->frames = frames;
    sprite->frame = 0;
    sprite->ticks = 0.0f;
    sprite->next = 1;
}

const char* sprite_state_from_direction(float direction) {
    int angle = (int)floor_ext(direction, 45);
    switch(angle) {
        case 45:
        case 90:
            return SPRITE_STATE_UP;
        case 135:
        case 180:
            return SPRITE_STATE_LEFT;
        case 225:
        case 270:
            return SPRITE_STATE_DOWN;
        case 315:
        case 360:
        case 0:
            return SPRITE_STATE_RIGHT;
    }

    printf("Invalid angle: %d\n", angle);
    return SPRITE_STATE_UP;
}

void sprite_system_update(EcsComponentSystem* system, float delta, void* component) {
    Sprite* sprite = component;
    if(rect_list_count(sprite->frames) == 1 || sprite->speed == 0)
        return;

    sprite->ticks += delta;
    if(sprite->ticks < sprite->speed)
        return;

    int next = sprite->frame + sprite->next;
    if(next < 0 || next >= rect_list_count(sprite->frames)) {
        switch(sprite->update_mode) {
            case SPRITE_UPDATE_CYCLE:
                next = 0;
                break;
            case SPRITE_UPDATE_PING_PONG:
                sprite->next *= -1;
                next = sprite->frame + sprite->next;
                break;
        }
    }

    sprite->frame = next;
    sprite->ticks -= sprite->speed;
}