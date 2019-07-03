#ifndef SKY_COLLECTIONS_H
#define SKY_COLLECTIONS_H

#include <list.h>
#include <map.h>

#include <ecs.h>
#include "data_types.h"

LIST_DEFINE_H(EntityList, entity_list, Entity)

LIST_DEFINE_H(RectList, rect_list, Rectangle)

MAP_DEFINE_H(AnimationSet, animation_set, char*, RectList*)

#endif