#ifndef SKY_SCENE_H
#define SKY_SCENE_H

#include "game_common.h"

#include "camera.h"
#include <ecs.h>
#include "game.h"

typedef struct Scene {
	Game* game;
	EcsSequentialSystem* update;
	EcsSequentialSystem* draw;
	EcsSequentialSystem* gui;
	Camera* camera;
	EcsWorld world;
} Scene;

Scene* scene_init(Game* game, EcsWorld world, Camera* camera, EcsSequentialSystem* update, EcsSequentialSystem* draw, EcsSequentialSystem* gui);
void scene_free(Scene* scene, bool free_systems, bool free_camera);
void scene_update(Scene* scene, float delta);
void scene_draw(Scene* scene, float delta);

void scene_push(Scene* scene);
void scene_change(Scene* scene);
Scene* scene_pop(bool free_scene);
Scene* scene_current(void);


#endif