#include <viewport.h>

#include <stdio.h>

#include <common_events.h>

Rectangle virtual_viewport;
Rectangle viewport;

static void viewport_on_window_resized(void* data, SDL_Window* window) {
	int width;
	int height;
	SDL_GetWindowSize(window, &width, &height);
	float xscale = (float)width / (float)virtual_viewport.w;
	float yscale = (float)height / (float)virtual_viewport.h;
	float scale = SDL_min(xscale, yscale);
	viewport.w = (int)(scale * (float)virtual_viewport.w + .5f);
	viewport.h = (int)(scale * (float)virtual_viewport.h + .5f);
	viewport.x = (int)((float)(width - viewport.w) * .5f);
	viewport.y = (int)((float)(height - viewport.h) * .5f);
}

void viewport_init(SDL_Window* window, Rectangle virt_viewport) {
	virtual_viewport = virt_viewport;
	viewport_on_window_resized(NULL, window);
	ecs_event_add(window_changed_event, ecs_closure(NULL, viewport_on_window_resized));
}