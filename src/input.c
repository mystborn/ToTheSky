#include <input.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_GAMEPADS 16
#define ACTION_KEY_COUNT 2

typedef struct Gamepad {
    SDL_GameController* controller;
    Uint32 button_current;
    Uint32 button_previous;
    bool active;
} Gamepad;

typedef struct ActionMap {
    SDL_Scancode keys[2];
    Uint32 button[2];
    MouseButton mouse;
    bool active;
} ActionMap;

typedef struct InputManager {
    MouseButton mouse_current;
    MouseButton mouse_previous;
    SDL_Point mouse_position_current;
    SDL_Point mouse_position_previous;
    const Uint8* keyboard_current;
    const Uint8* keyboard_previous;
    Gamepad gamepads[MAX_GAMEPADS];
    int controllers[MAX_GAMEPADS];
    int controller_count;
    Uint16 deadzone;
    ActionMap* maps;
    int action_count;
} InputManager;

static InputManager input_manager = {0};

#define GAMEPAD_BUTTON(x) (1 << (x))

bool key_check(SDL_Scancode key) {
    return input_manager.keyboard_current[key] == 1;
}

bool key_check_pressed(SDL_Scancode key) {
    return input_manager.keyboard_current[key] == 1 && input_manager.keyboard_previous[key] == 0;
}

bool key_check_released(SDL_Scancode key) {
    return input_manager.keyboard_current[key] == 0 && input_manager.keyboard_previous[key] == 1;
}

bool mouse_check(MouseButton button) {
    return (input_manager.mouse_current & button) == button;
}

bool mouse_check_pressed(MouseButton button) {
    return ((input_manager.mouse_current & button) == button) && ((input_manager.mouse_previous & button) != button);
}

bool mouse_check_released(MouseButton button) {
    return ((input_manager.mouse_current & button) != button) && ((input_manager.mouse_previous & button) == button);
}

bool mouse_moved(void) {
    return input_manager.mouse_position_current.x != input_manager.mouse_position_previous.x ||
           input_manager.mouse_position_current.y != input_manager.mouse_position_previous.y;
}

bool gamepad_check_index(Uint32 button, int index) {
    if(index == -1)
        index = input_manager.controllers[0];

    if(index < 0 || index >= MAX_GAMEPADS || !input_manager.gamepads[index].active)
        return false;

    return (input_manager.gamepads[index].button_current & GAMEPAD_BUTTON(button)) != 0;
}

bool gamepad_check_pressed_index(Uint32 button, int index) {
    if(index == -1)
        index = input_manager.controllers[0];

    if(index < 0 || index >= MAX_GAMEPADS || !input_manager.gamepads[index].active)
        return false;

    return (input_manager.gamepads[index].button_current & GAMEPAD_BUTTON(button)) != 0 &&
           (input_manager.gamepads[index].button_current & GAMEPAD_BUTTON(button)) == 0;
}

bool gamepad_check_released_index(Uint32 button, int index) {
    if(index == -1)
        index = input_manager.controllers[0];

    if(index < 0 || index >= MAX_GAMEPADS || !input_manager.gamepads[index].active)
        return false;

    return (input_manager.gamepads[index].button_current & GAMEPAD_BUTTON(button)) == 0 &&
           (input_manager.gamepads[index].button_current & GAMEPAD_BUTTON(button)) != 0;
}

Uint16 gamepad_axis_value_index(SDL_GameControllerAxis axis, int index) {
    if(index == -1)
        index = input_manager.controllers[0];

    if(index < 0 || index >= MAX_GAMEPADS || !input_manager.gamepads[index].active)
        return 0;

    return SDL_GameControllerGetAxis(input_manager.gamepads[index].controller, axis);
}

void gamepad_set_deadzone(Uint16 value) {
    input_manager.deadzone = value;
}

Uint16 gamepad_get_deadzone(void) {
    return input_manager.deadzone;
}

void action_set_key(Uint32 action, SDL_Scancode key, int key_index) {
    if(action >= input_manager.action_count)
        return;

    if(key_index < 0 || key_index > 1)
        return;

    input_manager.maps[action].keys[key_index] = key;
}

void action_set_button(Uint32 action, Sint32 button, int button_index) {
    if(action >= input_manager.action_count)
        return;

    if(button_index < 0 || button_index > 1)
        return;

    input_manager.maps[action].button[button_index] = button;
}

void action_set_mouse(Uint32 action, MouseButton button) {
    if(action >= input_manager.action_count)
        return;

    input_manager.maps[action].mouse = button;
}

bool action_check(Uint32 action) {
    if(action >= input_manager.action_count)
        return false;

    ActionMap map = input_manager.maps[action];

    for(int i = 0; i < ACTION_KEY_COUNT; i++)
        if(map.keys[i] != SDL_SCANCODE_UNKNOWN && key_check(map.keys[i]))
            return true;

    for(int i = 0; i < ACTION_KEY_COUNT; i++)
        if(map.button[i] != SDL_CONTROLLER_BUTTON_INVALID && gamepad_check(map.button[i]))
            return true;

    if(map.mouse != 0 && mouse_check(map.mouse))
        return true;

    return false;
}

bool action_check_pressed(Uint32 action) {
    if(action >= input_manager.action_count)
        return false;

    ActionMap map = input_manager.maps[action];

    for(int i = 0; i < ACTION_KEY_COUNT; i++)
        if(map.keys[i] != SDL_SCANCODE_UNKNOWN && key_check_pressed(map.keys[i]))
            return true;

    for(int i = 0; i < ACTION_KEY_COUNT; i++)
        if(map.button[i] != SDL_CONTROLLER_BUTTON_INVALID && gamepad_check_pressed(map.button[i]))
            return true;

    if(map.mouse != 0 && mouse_check_pressed(map.mouse))
        return true;

    return false;
}

bool action_check_released(Uint32 action) {
    if(action >= input_manager.action_count)
        return false;

    ActionMap map = input_manager.maps[action];

    for(int i = 0; i < ACTION_KEY_COUNT; i++)
        if(map.keys[i] != SDL_SCANCODE_UNKNOWN && key_check_released(map.keys[i]))
            return true;

    for(int i = 0; i < ACTION_KEY_COUNT; i++)
        if(map.button[i] != SDL_CONTROLLER_BUTTON_INVALID && gamepad_check_released(map.button[i]))
            return true;

    if(map.mouse != 0 && mouse_check_released(map.mouse))
        return true;

    return false;
}

static void gamepad_update(Gamepad* gamepad) {
    gamepad->button_previous = gamepad->button_current;
    gamepad->button_current = 0;

    for(int i = SDL_CONTROLLER_BUTTON_A; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        if(SDL_GameControllerGetButton(gamepad->controller, i))
            gamepad->button_current |= GAMEPAD_BUTTON(i);
    }

    for(int i = SDL_CONTROLLER_AXIS_LEFTX; i < SDL_CONTROLLER_AXIS_MAX; i++) {
        Sint16 axis = SDL_GameControllerGetAxis(gamepad->controller, i);
        if(axis < -input_manager.deadzone || axis > input_manager.deadzone) {
            int index;
            switch(i) {
                case SDL_CONTROLLER_AXIS_LEFTX:
                    index = axis < 0 ? SDL_CONTROLLER_BUTTON_LEFTSTICKLEFT : SDL_CONTROLLER_BUTTON_LEFTSTICKRIGHT;
                    break;
                case SDL_CONTROLLER_AXIS_LEFTY:
                    index = axis < 0 ? SDL_CONTROLLER_BUTTON_LEFTSTICKUP : SDL_CONTROLLER_BUTTON_LEFTSTICKDOWN;
                    break;
                case SDL_CONTROLLER_AXIS_RIGHTX:
                    index = axis < 0 ? SDL_CONTROLLER_BUTTON_RIGHTSTICKLEFT : SDL_CONTROLLER_BUTTON_RIGHTSTICKRIGHT;
                    break;
                case SDL_CONTROLLER_AXIS_RIGHTY:
                    index = axis < 0 ? SDL_CONTROLLER_BUTTON_RIGHTSTICKUP : SDL_CONTROLLER_BUTTON_RIGHTSTICKDOWN;
                    break;
                case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
                    index = SDL_CONTROLLER_BUTTON_LEFTTRIGGER;
                    break;
                case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
                    index = SDL_CONTROLLER_BUTTON_RIGHTTRIGGER;
                    break;
            }
            gamepad->button_current |= GAMEPAD_BUTTON(index);
        }
    }
}

static void gamepad_open(int index) {
    SDL_GameController* controller = SDL_GameControllerOpen(index);

    if(!controller)
        return;

    Gamepad* gp = input_manager.gamepads + index;

    gp->controller = controller;
    gp->active = true;
    gamepad_update(gp);
    input_manager.controllers[input_manager.controller_count++] = index;
}

bool input_manager_init(int action_count) {
    if(!SDL_WasInit(SDL_INIT_GAMECONTROLLER)) {
        if(SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0)
            return false;
    }

    if((input_manager.maps = malloc(action_count * sizeof(ActionMap))) == NULL) {
        SDL_SetError("Could not initialize input_manager, not enough memory for actions.");
        return false;
    }

    for(int i = 0; i < action_count; i++) {
        ActionMap map = input_manager.maps[i];
        map.keys[0] = SDL_SCANCODE_UNKNOWN;
        map.keys[1] = SDL_SCANCODE_UNKNOWN;
        map.button[0] = SDL_CONTROLLER_BUTTON_INVALID;
        map.button[1] = SDL_CONTROLLER_BUTTON_INVALID;
        map.mouse = 0;
    }

    input_manager.deadzone = SHRT_MAX * .15f;

    input_manager.mouse_current = SDL_GetMouseState(&input_manager.mouse_position_current.x, &input_manager.mouse_position_current.y);

    input_manager.keyboard_current = SDL_GetKeyboardState(NULL);

    return true;
}

void input_manager_update(void) {
    input_manager.mouse_previous = input_manager.mouse_current;
    input_manager.mouse_position_previous = input_manager.mouse_position_current;
    input_manager.keyboard_previous = input_manager.keyboard_current;

    input_manager.mouse_current = SDL_GetMouseState(&input_manager.mouse_position_current.x, &input_manager.mouse_position_current.y);
    input_manager.keyboard_current = SDL_GetKeyboardState(NULL);

    for(int i = 0; i < input_manager.controller_count; i++)
        gamepad_update(input_manager.gamepads + i);
}

void input_manager_event(SDL_ControllerDeviceEvent* event) {
    switch(event->type) {
        case SDL_CONTROLLERDEVICEADDED:
            gamepad_open(event->which);
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
        {
            for(int i = 0; i < MAX_GAMEPADS; i++) {
                if(input_manager.controllers[i] == event->which) {
                    if(i != input_manager.controller_count - 1)
                        memmove(input_manager.controllers + i, input_manager.controllers + i + 1, (input_manager.controller_count - i - 1) * sizeof(Gamepad));
                    --input_manager.controller_count;
                    input_manager.controllers[input_manager.controller_count] = -1;
                    return;
                }
            }
            break;
        }
    }
}

void input_manager_free(void) {
    free(input_manager.maps);
    input_manager.maps = NULL;
}