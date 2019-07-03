#include <scene.h>
#include <colors.h>
#include <game.h>
#include <viewport.h>

struct SceneManager {
	Scene** scenes;
	int capacity;
	int count;
};

static struct SceneManager scene_manager = { .scenes = NULL, .capacity = 0, .count = 0 };

Scene* scene_init(Game* game, EcsWorld world, Camera* camera, EcsSequentialSystem* update, EcsSequentialSystem* draw, EcsSequentialSystem* gui) {
	Scene* scene = malloc(sizeof(Scene));
	if (scene == NULL)
		return NULL;
	scene->game = game;
	scene->world = world;
	scene->camera = camera;
	scene->update = update;
	scene->draw = draw;
	scene->gui = gui;
    return scene;
}

void scene_free(Scene* scene, bool free_systems, bool free_camera) {
	if (free_systems) {
		ecs_system_free_resources(scene->update);
		ecs_system_free_resources(scene->draw);
		ecs_system_free_resources(scene->gui);
		free(scene->update);
		free(scene->draw);
		free(scene->gui);
	}
	if (free_camera) {
		camera_free(scene->camera);
	}
	ecs_world_free(scene->world);
	free(scene);
}

void scene_update(Scene* scene, float delta) {
	ecs_system_update(scene->update, delta);
}

void scene_draw(Scene* scene, float delta) {
	Texture* render_target = camera_get_render_target(scene->camera);
	SDL_SetRenderTarget(renderer, render_target);
	Uint8 r, g, b, a;

	SDL_GetRGBA(game_get_background(scene->game), game_pixel_format, &r, &g, &b, &a);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderClear(renderer);
	SDL_RenderSetViewport(renderer, NULL);

	ecs_system_update(scene->draw, delta);

	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);
	SDL_RenderClear(renderer);

	SDL_RenderSetViewport(renderer, &viewport);

	Point size = camera_get_size(scene->camera);

	SDL_RenderCopyEx(renderer, 
		             render_target, 
		             &(Rectangle){ 0, 0, size.x, size.y }, 
		             &(Rectangle){ 0, 0, viewport.w, viewport.h }, 
		             camera_get_rotation(scene->camera), 
		             NULL, 
		             SDL_FLIP_NONE);

	ecs_system_update(scene->gui, delta);

	SDL_RenderPresent(renderer);
}

void scene_push(Scene* scene) {
	ECS_ARRAY_RESIZE_DEFAULT(scene_manager.scenes, scene_manager.capacity, scene_manager.count + 1, sizeof(Scene*), NULL);
	scene_manager.scenes[scene_manager.count++] = scene;
}

void scene_change(Scene* scene) {
	while (scene_manager.count > 0)
		scene_pop(true);
}

Scene* scene_pop(bool free_scene) {
	if (scene_manager.count == 0)
		return NULL;

	Scene* scene = scene_manager.scenes[--scene_manager.count];

	if(free_scene)
		scene_free(scene, true, true);

	return scene;
}

Scene* scene_current(void) {
	if(scene_manager.count == 0)
		return NULL;

	return scene_manager.scenes[scene_manager.count - 1];
}

