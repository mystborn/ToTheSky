#include <timer.h>
#include <stdlib.h>

struct Timer {
	Uint32 start_ticks;
	Uint32 paused_ticks;
	bool paused;
	bool started;
};


Timer* timer_init(void) {
	Timer* timer = malloc(sizeof(Timer));
	if (timer == NULL)
		return NULL;
	timer->start_ticks = 0;
	timer->paused_ticks = 0;
	timer->paused = false;
	timer->started = false;
	return timer;
}

void timer_free(Timer* timer) {
	free(timer);
}

void timer_start(Timer* timer) {
	timer->started = true;
	timer->paused = false;
	timer->paused_ticks = 0;
	timer->start_ticks = SDL_GetTicks();
}

void timer_stop(Timer* timer) {
	timer->start_ticks = 0;
	timer->paused_ticks = 0;
	timer->paused = false;
	timer->started = false;
}

void timer_pause(Timer* timer) {
	if (timer->started && !timer->paused) {
		timer->paused_ticks = SDL_GetTicks() - timer->start_ticks;
		timer->paused = true;
		timer->start_ticks = 0;
	}
}

void timer_resume(Timer* timer) {
	if (timer->started && timer->paused) {
		timer->paused = false;
		timer->start_ticks = SDL_GetTicks() - timer->paused_ticks;
		timer->paused_ticks = 0;
	}
}

Uint32 timer_ticks(Timer* timer) {
	Uint32 time = 0;
	if (timer->started) {
		if (timer->paused)
			time = timer->paused_ticks;
		else
			time = SDL_GetTicks() - timer->start_ticks;
	}

	return time;
}

bool timer_started(Timer* timer) {
	return timer->started;
}

bool timer_paused(Timer* timer) {
	return timer->paused;
}
