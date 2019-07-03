#ifndef SKY_RECTF_H
#define SKY_RECTF_H

#include <stdbool.h>
#include "data_types.h"

typedef struct RectangleF {
    float x;
    float y;
    float w;
    float h;
} RectangleF;

static inline bool rectf_contains_vector(const RectangleF* rect, const Vector2 value) {
    return rect->x <= value.x && 
           value.x <= rect->x + rect->w && 
           rect->y <= value.y && 
           value.y <= rect->y + rect->h; 
}

static inline bool rectf_intersects(const RectangleF* left, const RectangleF* right) {
    return right->x < left->x + left->w &&
           left->x < right->x + right->w &&
           right->y < left->y + left->h &&
           left->y < right->y + right->h;
}

#endif