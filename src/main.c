#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <game_common.h>

#include <colors.h>
#include <common_events.h>
#include <content_loader.h>
#include <fonts.h>
#include <game.h>
#include <render_system.h>
#include <scene.h>
#include <sprite.h>
#include <viewport.h>
#include <input.h>

#define INIT_DEBUG(...) __VA_ARGS__, printf(#__VA_ARGS__ "\n")

#define INIT_DEBUG_RESULT(result, ...) result = __VA_ARGS__, printf(#__VA_ARGS__ "\n")

static void draw_fps(EcsActionSystem* system, float delta) {
    static int i = 0;
    static float delta_over_time[120];
    static float calculated_delta = 0;
    if(i == 120) {
        i = 0;
    }
    delta_over_time[i++] = delta;
    if(i % 15 == 0) {
        for(int i = 0; i < 120; i++)
            calculated_delta += delta_over_time[i];

        calculated_delta /= 120.0f;
    }

    //FC_Draw(f_text, renderer, 0, 0, "FPS: %g", delta / (1/60));
    FC_Draw(f_text, renderer, 0, 0, "FPS: %g", 1.0f / calculated_delta);
}

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER) < 0) {
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		return EXIT_FAILURE;
	}

	int img_flags = IMG_INIT_PNG;
	if (!(IMG_Init(img_flags) & img_flags)) {
		printf("SDL_image could not initialize! SDL_Error: %s\n", IMG_GetError());
        SDL_Quit();
		return EXIT_FAILURE;
	}

    if (TTF_Init() == -1) {
        printf("SDL_ttf could not initialize! SDL_Error: %s\n", TTF_GetError());
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_GameControllerAddMapping("controller_mappings.txt");

    INIT_DEBUG(ecs_init());
    INIT_DEBUG(common_events_init());
    INIT_DEBUG(colors_init(SDL_PIXELFORMAT_RGBA8888));
    INIT_DEBUG(content_loader_init());
    INIT_DEBUG(input_manager_init(0));

	int width = 800;
	int height = 450;
    Game* game;
    INIT_DEBUG_RESULT(game, game_init("To the Sky", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE));

    if (game == NULL) {
        printf("Game could not be initialized! SDL_Error: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    if(!fonts_init()) {
        printf("Fonts could not be initialized! SDL_Error: %s\n", TTF_GetError());
        TTF_Quit();
        IMG_Quit();
        SDL_Quit();
        return EXIT_FAILURE;
    }

	INIT_DEBUG(viewport_init(game->window, (Rectangle) { 0, 0, width, height }));

    EcsWorld world;
    INIT_DEBUG_RESULT(world, ecs_world_init());

    Camera* camera = camera_init(renderer, width / 2, height / 2);
    EcsSequentialSystem *update, *draw, *gui;
    update = malloc(sizeof(EcsSequentialSystem));
    draw = malloc(sizeof(EcsSequentialSystem));
    gui = malloc(sizeof(EcsSequentialSystem));
    EcsComponentSystem* sprite_update = malloc(sizeof(EcsComponentSystem));
    EcsActionSystem* fps = malloc(sizeof(EcsActionSystem));
    RenderSystem* render_system = malloc(sizeof(RenderSystem));
    ecs_component_system_init(sprite_update, world, sprite_component, sprite_system_update, NULL, NULL);
    ecs_action_system_init(fps, draw_fps, NULL, NULL);
    render_system_init(render_system, world, camera);
    ecs_sequential_system_init(update, NULL, NULL, true, 1, sprite_update);
    ecs_sequential_system_init(draw, NULL, NULL, true, 1, render_system);
    ecs_sequential_system_init(gui, NULL, NULL, true, 1, fps);

    Scene* scene;
    INIT_DEBUG_RESULT(scene, scene_init(game, world, camera, update, draw, gui));

    scene_push(scene);

    Animation* player_animation = animation_load("animations/player.anim");
    if(player_animation == NULL) {
        printf("%s\n", SDL_GetError());
        goto SKIP_GAME_RUN;
    }

    Entity player = ecs_create_entity(world);
    Sprite* sprite = ecs_component_set(player, sprite_component);
    Vector2* position = ecs_component_set(player, position_component);

    INIT_DEBUG(sprite_init(sprite, player_animation));
    position->x = 100;
    position->y = 100;

	if(!game_run(game)) {
        printf("%s\n", SDL_GetError());
    }

    INIT_DEBUG(scene_pop(true));

    SKIP_GAME_RUN:

	INIT_DEBUG(game_destroy(game));

    INIT_DEBUG(texture_free_all());
    INIT_DEBUG(animation_free_all());

    INIT_DEBUG(fonts_free());

    INIT_DEBUG(IMG_Quit());
    INIT_DEBUG(TTF_Quit());
    INIT_DEBUG(SDL_Quit());

	return EXIT_SUCCESS;
}