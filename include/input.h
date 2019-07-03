#ifndef INPUT_MANAGER_INPUT_MANAGER_H
#define INPUT_MANAGER_INPUT_MANAGER_H

#include <stdbool.h>
#include <SDL.h>

typedef enum SDL_GameControllerButtonExtension {
    SDL_CONTROLLER_BUTTON_EXTENSION_INVALID = -1,
    SDL_CONTROLLER_BUTTON_LEFTSTICKUP = SDL_CONTROLLER_BUTTON_MAX,
    SDL_CONTROLLER_BUTTON_LEFTSTICKLEFT,
    SDL_CONTROLLER_BUTTON_LEFTSTICKDOWN,
    SDL_CONTROLLER_BUTTON_LEFTSTICKRIGHT,
    SDL_CONTROLLER_BUTTON_RIGHTSTICKUP,
    SDL_CONTROLLER_BUTTON_RIGHTSTICKLEFT,
    SDL_CONTROLLER_BUTTON_RIGHTSTICKDOWN,
    SDL_CONTROLLER_BUTTON_RIGHTSTICKRIGHT,
    SDL_CONTROLLER_BUTTON_LEFTTRIGGER,
    SDL_CONTROLLER_BUTTON_RIGHTTRIGGER
} SDL_GameControllerButtonExtension;

typedef Uint32 MouseButton;

bool key_check(SDL_Scancode key);
bool key_check_pressed(SDL_Scancode key);
bool key_check_released(SDL_Scancode key);

bool mouse_check(MouseButton button);
bool mouse_check_pressed(MouseButton button);
bool mouse_check_released(MouseButton button);
bool mouse_moved(void);

bool gamepad_check_index(Uint32 button, int index);
bool gamepad_check_pressed_index(Uint32 button, int index);
bool gamepad_check_released_index(Uint32 button, int index);
Uint16 gamepad_axis_value_index(SDL_GameControllerAxis axis, int index);

void gamepad_set_deadzone(Uint16 value);
Uint16 gamepad_get_deadzone(void);

#define gamepad_check(button) gamepad_check_index((button), -1)
#define gamepad_check_pressed(button) gamepad_check_pressed_index((button), -1)
#define gamepad_check_released(button) gamepad_check_released_index((button), -1)
#define gamepad_axis_value(axis) gamepad_axis_value_index((axis), -1)

void action_set_key(Uint32 action, SDL_Scancode key, int key_index);
void action_set_button(Uint32 action, Sint32 button, int button_index);
void action_set_axis(Uint32 action, SDL_GameControllerButtonExtension axis);
void action_set_mouse(Uint32 action, MouseButton button);

bool action_check(Uint32 action);
bool action_check_pressed(Uint32 action);
bool action_check_released(Uint32 action);

bool input_manager_init(int action_count);
void input_manager_update(void);
void input_manager_event(SDL_ControllerDeviceEvent* event);
void input_manager_free(void);

#endif