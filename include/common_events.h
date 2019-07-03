#ifndef SKY_COMMON_EVENTS_H
#define SKY_COMMON_EVENTS_H

#include <ecs.h>

// Triggered when the game window is resized, either by the user or by program code.
// function signature: void (*)(SDL_Window*)
extern EcsEvent* window_changed_event;

extern ComponentManager* sprite_component;
extern ComponentManager* position_component;
extern ComponentManager* collider_component;
extern ComponentManager* velocity_component;
extern ComponentManager* player_movement_component;
extern ComponentManager* gui_component;

void common_events_init(void);

#endif // !SKY_COMMON_EVENTS_H