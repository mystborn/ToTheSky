#ifndef SKY_CAMERA_H
#define SKY_CAMERA_H

#include <stdbool.h>

#include <SDL.h>
#include "data_types.h"

typedef struct Camera Camera;

Camera* camera_init(SDL_Renderer* renderer, int width, int height);
void camera_free(Camera* camera);
void camera_move(Camera* camera, Point direction);
void camera_rotate(Camera* camera, double delta_degrees);
void camera_lookat(Camera* camera, Point position);
void camera_set_position(Camera* camera, Point position);
void camera_set_x(Camera* camera, int x);
void camera_set_y(Camera* camera, int y);

/// <summary>
/// Sets the size of the camera in the game world.
/// Do not call this during a draw event.
/// </summary>
bool camera_set_size(Camera* camera, Point size);

/// <summary>
/// Sets the width of the camera in the game world.
/// Do not call this during a draw event.
/// </summary>
bool camera_set_width(Camera* camera, int width);

/// <summary>
/// Sets the height of the camera in the game world.
/// Do not call this during a draw event.
/// </summary>
bool camera_set_height(Camera* camera, int height);
void camera_set_rotation(Camera* camera, double degrees);
Point camera_get_position(Camera* camera);
int camera_get_x(Camera* camera);
int camera_get_y(Camera* camera);
Point camera_get_size(Camera* camera);
int camera_get_width(Camera* camera);
int camera_get_height(Camera* camera);
double camera_get_rotation(Camera* camera);
Rectangle camera_get_bounds(Camera* camera);
Texture* camera_get_render_target(Camera* camera);
Point camera_screen_to_world(Camera* camera, Point screen_position);
Point camera_world_to_screen(Camera* camera, Point world_position);


#endif