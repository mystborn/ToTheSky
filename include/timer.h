#ifndef SKY_TIMER_H
#define SKY_TIMER_H

#include "game_common.h"

typedef struct Timer Timer;

Timer* timer_init(void);
void timer_free(Timer* timer);
void timer_start(Timer* timer);
void timer_stop(Timer* timer);
void timer_pause(Timer* timer);
void timer_resume(Timer* timer);
Uint32 timer_ticks(Timer* timer);
bool timer_started(Timer* timer);
bool timer_paused(Timer* timer);

#endif