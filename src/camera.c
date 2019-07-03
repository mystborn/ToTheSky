#include <camera.h>

#include <colors.h>
#include <ecs.h>
#include <viewport.h>

struct Camera {
	Rectangle view;
	double rotation;
	SDL_Renderer* renderer;
	Texture* render_target;
};

Camera* camera_init(SDL_Renderer* renderer, int width, int height) {
	Camera* camera = malloc(sizeof(Camera));
	camera->rotation = 0;
	camera->view.x = 0;
	camera->view.y = 0;
	camera->view.w = width;
	camera->view.h = height;
	camera->renderer = renderer;
	camera->render_target = SDL_CreateTexture(renderer, game_pixel_format->format, SDL_TEXTUREACCESS_TARGET, width, height);
	return camera;
}

void camera_free(Camera* camera) {
	SDL_DestroyTexture(camera->render_target);
	free(camera);
}

void camera_move(Camera* camera, Point direction) {
	camera->view.x += direction.x;
	camera->view.y += direction.y;
}

void camera_rotate(Camera* camera, double delta_degrees) {
	camera->rotation += delta_degrees;
}

void camera_lookat(Camera* camera, Point position) {
	camera->view.x = position.x - camera->view.w / 2;
	camera->view.y = position.y - camera->view.h / 2;
}

void camera_set_position(Camera* camera, Point position) {
	camera->view.x = position.x;
	camera->view.y = position.y;
}

void camera_set_x(Camera* camera, int x) {
	camera->view.x = x;
}

void camera_set_y(Camera* camera, int y) {
	camera->view.y = y;
}

bool camera_set_size(Camera* camera, Point size) {
	// Early exit if the size didn't change in order to avoid
	// expensive reallocation of SDL_Texture.
	if (camera->view.w == size.x && camera->view.h == size.y)
		return true;

	camera->view.w = size.x;
	camera->view.h = size.y;

	SDL_Texture* texture = SDL_CreateTexture(camera->renderer, game_pixel_format->format, SDL_TEXTUREACCESS_TARGET, size.x, size.y);
	if (texture == NULL)
		return false;

	SDL_DestroyTexture(camera->render_target);
	camera->render_target = texture;

	return true;
}

bool camera_set_width(Camera* camera, int width) {
	if (camera->view.w == width)
		return true;

	camera->view.w = width;

	SDL_Texture* texture = SDL_CreateTexture(camera->renderer, game_pixel_format->format, SDL_TEXTUREACCESS_TARGET, width, camera->view.h);
	if (texture == NULL)
		return false;

	SDL_DestroyTexture(camera->render_target);
	camera->render_target = texture;

	return true;
}

bool camera_set_height(Camera* camera, int height) {
	if (camera->view.h == height)
		return true;

	camera->view.h = height;

	SDL_Texture* texture = SDL_CreateTexture(camera->renderer, game_pixel_format->format, SDL_TEXTUREACCESS_TARGET, camera->view.w, height);
	if (texture == NULL)
		return false;

	SDL_DestroyTexture(camera->render_target);
	camera->render_target = texture;

	return true;
}

void camera_set_rotation(Camera* camera, double degrees) {
	camera->rotation = degrees;
}

Point camera_get_position(Camera* camera) {
	return (Point) { camera->view.x, camera->view.y };
}

int camera_get_x(Camera* camera) {
	return camera->view.x;
}

int camera_get_y(Camera* camera) {
	return camera->view.y;
}

Point camera_get_size(Camera* camera) {
	return (Point) { camera->view.w, camera->view.h };
}

int camera_get_width(Camera* camera) {
	return camera->view.w;
}

int camera_get_height(Camera* camera) {
	return camera->view.h;
}

double camera_get_rotation(Camera* camera) {
	return camera->rotation;
}

Rectangle camera_get_bounds(Camera* camera) {
	return camera->view;
}

Texture* camera_get_render_target(Camera* camera) {
	return camera->render_target;
}

Point camera_screen_to_world(Camera* camera, Point screen_position) {
	screen_position.x = (int)((float)(screen_position.x - viewport.x) * ((float)camera->view.w / (float)viewport.w)) + camera->view.x;
	screen_position.y = (int)((float)(screen_position.y - viewport.y) * ((float)camera->view.h / (float)viewport.h)) + camera->view.y;
	return screen_position;
}

Point camera_world_to_screen(Camera* camera, Point world_position) {
	world_position.x = (int)((float)(world_position.x - camera->view.x) * ((float)viewport.w / (float)camera->view.w)) + viewport.x;
	world_position.y = (int)((float)(world_position.y - camera->view.y) * ((float)viewport.h / (float)camera->view.h)) + viewport.y;
	return world_position;
}