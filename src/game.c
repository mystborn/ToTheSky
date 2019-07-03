#include <game.h>

#include <stdio.h>

#include <colors.h>
#include <common_events.h>
#include <scene.h>
#include <input.h>

#define GAME_FPS 60
#define GAME_TICKS_PER_FRAME 1000 / GAME_FPS

SDL_Renderer* renderer;
static Game current_game;
static bool current_game_initialized = false;
static int game_on_window_resized_event_index;

static void game_on_window_resized(void* data, SDL_Window* window) {
	int display_index = SDL_GetWindowDisplayIndex(current_game.window);
	SDL_GetCurrentDisplayMode(display_index, &current_game.display_mode);
    printf("Moniter %d has a refresh rate of %d\n", display_index, current_game.display_mode.refresh_rate);
}

Game* game_init(char* title, int x, int y, int width, int height, Uint32 window_flags) {
	if (current_game_initialized)
		game_destroy(&current_game);

	current_game.window = SDL_CreateWindow(title, x, y, width, height, window_flags);
	if (current_game.window == NULL) {
		return NULL;
	}

	renderer = SDL_CreateRenderer(current_game.window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == NULL) {
		SDL_DestroyWindow(current_game.window);
		return NULL;
	}

	game_on_window_resized(NULL, current_game.window);
    ecs_event_add(window_changed_event, ecs_closure(NULL, game_on_window_resized));

	current_game_initialized = true;
	current_game.background_color = c_white;

	return &current_game;
}

void game_destroy(Game* game) {
    if (!current_game_initialized)
        return;

	SDL_DestroyRenderer(renderer);
    ecs_event_remove(window_changed_event, game_on_window_resized_event_index);
	SDL_DestroyWindow(game->window);
	current_game_initialized = false;
}

bool game_run(Game* game) {
	bool quit = false;
	SDL_Event e;

	Timer* fps_timer = timer_init();
	if (fps_timer == NULL) {
		SDL_SetError("Failure to initialize fps timer");
		return false;
	}

	Timer* cap_timer = timer_init();
	if (cap_timer == NULL) {
		SDL_SetError("Failed to initialize frame cap timer");
		timer_free(fps_timer);
		return false;
	}

	timer_start(fps_timer);
	Uint32 prev_time = 0;
	float delta = 0;

	while (!quit) {
		timer_start(cap_timer);

		while (SDL_PollEvent(&e)) {
			switch (e.type) {
			    case SDL_QUIT:
                    quit = true;
			    	break;
                case SDL_CONTROLLERDEVICEADDED:
                case SDL_CONTROLLERDEVICEREMOVED:
                    input_manager_event(&e.cdevice);
                    break;
                case SDL_WINDOWEVENT:
                    if (SDL_GetWindowID(current_game.window) != e.window.windowID)
                        break;
    
                        switch (e.window.event) {
                            case SDL_WINDOWEVENT_MAXIMIZED:
                            case SDL_WINDOWEVENT_MOVED:
                            case SDL_WINDOWEVENT_RESIZED:
                            case SDL_WINDOWEVENT_RESTORED:
                            case SDL_WINDOWEVENT_SHOWN:
                            case SDL_WINDOWEVENT_SIZE_CHANGED:
                                // When the window dimensions are changed for a visible reason,
                                // call all window changed event handlers.
                                ecs_event_trigger(window_changed_event, void (*)(void*, SDL_Window*), current_game.window);
                                break;
                    }
                    break;
			}
		}

        if(quit) {
            puts("quitting");
            break;
        }

        input_manager_update();

		Uint32 temp = timer_ticks(fps_timer);
		delta = (temp - prev_time) / 1000.f;
		prev_time = temp;


        // Keep updating the scene until the scene doesn't change.
		Scene* scene;
		do {
			scene = scene_current();
			if (scene == NULL)
				break;

            scene_update(scene, delta);
        } while (scene != scene_current());

        if (scene != NULL) {
            scene_draw(scene, delta);
        }

		int frame_ticks = timer_ticks(cap_timer);
		if (frame_ticks < GAME_TICKS_PER_FRAME)
			SDL_Delay(GAME_TICKS_PER_FRAME - frame_ticks);
	}

    puts("hello?");

	return true;
}

Uint32 game_get_background(Game* game) {
	return game->background_color;
}

void game_set_background(Game* game, Uint32 color) {
	game->background_color = color;
}