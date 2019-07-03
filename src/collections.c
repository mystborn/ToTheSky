#include <collections.h>

#include <string.h>

LIST_DEFINE_C(EntityList, entity_list, Entity)

LIST_DEFINE_C(RectList, rect_list, Rectangle)

MAP_DEFINE_C(AnimationSet, animation_set, char*, RectList*, fnv32, strcmp, NULL)