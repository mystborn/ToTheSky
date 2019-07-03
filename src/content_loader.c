#include <content_loader.h>

#include <string.h>
#include <stdlib.h>

#include <game.h>
#include <map.h>
#include <cJSON.h>

MAP_DEFINE_H(Content, content, char*, void*)

MAP_DEFINE_C(Content, content, char*, void*, fnv32, strcmp, NULL)

static Content textures;
static Content animations;

void content_loader_init(void) {
    content_init(&textures);
    content_init(&animations);
}

SDL_Texture* texture_load(char* path) {
    SDL_Texture* result = content_get(&textures, path);
    if(result == NULL) {
        SDL_Surface* surface = IMG_Load(path);
        if(surface == NULL)
            return NULL;

        result = SDL_CreateTextureFromSurface(renderer, surface);

        SDL_FreeSurface(surface);

        if(result == NULL)
            return NULL;

        char* key = malloc(sizeof(char) * strlen(path) + 1);
        strcpy(key, path);

        content_set(&textures, key, result);
    }
    return result;
}

void texture_free(SDL_Texture* texture) {
    for(int i = 0; i < textures.capacity; i++) {
        if(textures.cells[i].active && textures.cells[i].value == (void*)texture) {
            ContentCell* cell = textures.cells + i;
            content_remove(&textures, cell->key);
            free(cell->key);
            break;
        }
    }

    SDL_DestroyTexture(texture);
}

void texture_free_all(void) {
    ContentCell* cells = textures.cells;
    uint32_t cap = textures.capacity;

    for(uint32_t i = 0; i < cap; i++) {
        if(cells[i].active) {
            SDL_DestroyTexture(cells[i].value);
            free(cells[i].key);
        }
    }

    content_free(&textures);
}

Animation* animation_load(char* path) {
    Animation* animation = content_get(&animations, path);
    if(animation != NULL)
        return animation;

    FILE* f = fopen(path, "r");
    if(f == NULL) {
        SDL_SetError("Could not load Animation [%s]! Could not open file.", path);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char* contents = malloc(fsize + 1);
    fread(contents, 1, fsize, f);
    fclose(f);

    contents[fsize] = '\0';
    SDL_Texture* texture = NULL;
    AnimationSet* set = NULL;

    cJSON* json = cJSON_Parse(contents);
    if(json == NULL) {
        const char* error_ptr = cJSON_GetErrorPtr();
        if(error_ptr != NULL)
            SDL_SetError("Could not load Animation [%s]! Error parsing JSON before %s", path, error_ptr);
        else
            SDL_SetError("Could not load Animation [%s]! Error parsing JSON", path);

        goto parse_error;
    }

    cJSON* temp = cJSON_GetObjectItemCaseSensitive(json, "texture");
    if(!cJSON_IsString(temp)) {
        SDL_SetError("Could not load Animation [%s]! texture field was not a string", path);
        goto parse_error;
    }

    SDL_Surface* surface = IMG_Load(temp->valuestring);
    if(surface == NULL)
        goto parse_error;

    texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

    if(texture == NULL)
        goto parse_error;

    temp = cJSON_GetObjectItemCaseSensitive(json, "width");
    if(!cJSON_IsNumber(temp)) {
        SDL_SetError("Could not load Animation [%s]! width field was not a number", path);
        goto parse_error;
    }

    int width = temp->valueint;

    temp = cJSON_GetObjectItemCaseSensitive(json, "height");
    if(!cJSON_IsNumber(temp)) {
        SDL_SetError("Could not load Animation [%s]! height field was not a number", path);
        goto parse_error;
    }

    int height = temp->valueint;

    cJSON* origin = cJSON_GetObjectItemCaseSensitive(json, "origin");
    if(!cJSON_IsObject(origin)) {
        SDL_SetError("Could not load Animation [%s]! origin field was not an object", path);
        goto parse_error;
    }

    Point origin_value;
    temp = cJSON_GetObjectItemCaseSensitive(origin, "x");
    if(!cJSON_IsNumber(temp)) {
        SDL_SetError("Could not load Animation [%s]! origin.x field was not a number", path);
        goto parse_error;
    }

    origin_value.x = temp->valueint;

    temp = cJSON_GetObjectItemCaseSensitive(origin, "y");
    if(!cJSON_IsNumber(temp)) {
        SDL_SetError("Could not load Animation [%s]! origin.y field was not a number", path);
        goto parse_error;
    }

    origin_value.y = temp->valueint;

    temp = cJSON_GetObjectItemCaseSensitive(json, "update_mode");
    if(!cJSON_IsString(temp)) {
        SDL_SetError("Could not load Animation [%s]! update_mode field was not a string", path);
        goto parse_error;
    }

    SpriteUpdateMode update_mode = strcmp("pingpong", temp->valuestring) == 0 ? SPRITE_UPDATE_PING_PONG : SPRITE_UPDATE_CYCLE;

    temp = cJSON_GetObjectItemCaseSensitive(json, "speed");
    if(!cJSON_IsNumber(temp)) {
        SDL_SetError("Could not load Animation [%s]! speed field was not a number", path);
        goto parse_error;
    }

    float speed = (float)temp->valuedouble;
    cJSON* states = cJSON_GetObjectItemCaseSensitive(json, "states");
    if(!cJSON_IsObject(states)) {
        SDL_SetError("Could not load Animation [%s]! states field was not an object", path);
        goto parse_error;
    }

    set = malloc(sizeof(AnimationSet));
    animation_set_init(set);

    char* default_state = NULL;

    cJSON_ArrayForEach(temp, states) {
        if(!cJSON_IsArray(temp)) {
            SDL_SetError("Could not load Animation [%s]! states.%s field was not an array", path, temp->string);
            goto parse_error;
        }

        RectList* frames = rect_list_create();

        cJSON* frame;
        int i = 0;
        cJSON_ArrayForEach(frame, temp) {
            SDL_Rect value = { 0, 0, width, height };

            if(!cJSON_IsObject(frame)) {
                rect_list_free(frames);
                free(frames);
                SDL_SetError("Could not load Animation [%s]! states.%s[%d] field was not an object", path, temp->string, i);
                goto parse_error;
            }

            cJSON* dimension = cJSON_GetObjectItemCaseSensitive(frame, "x");
            if(!cJSON_IsNumber(dimension)) {
                rect_list_free(frames);
                free(frames);
                SDL_SetError("Could not load Animation [%s]! states.%s[%d].x field was not an object", path, temp->string, i);
                goto parse_error;
            }

            value.x = dimension->valueint;

            dimension = cJSON_GetObjectItemCaseSensitive(frame, "y");
            if(!cJSON_IsNumber(dimension)) {
                rect_list_free(frames);
                free(frames);
                SDL_SetError("Could not load Animation [%s]! states.%s[%d].y field was not an object", path, temp->string, i);
                goto parse_error;
            }

            value.y = dimension->valueint;

            rect_list_add(frames, value);

            i++;
        }

        int length = strlen(temp->string) + 1;
        char* key = malloc(length);
        strcpy(key, temp->string);

        if(default_state == NULL)
            default_state = key;

        if(!animation_set_add(set, key, frames)) {
            rect_list_free(frames);
            free(frames);
            free(key);
            SDL_SetError("Could not load Animation [%s]! states.%s field was a duplicate", path, temp->string);
            goto parse_error;
        }
    }

    animation = malloc(sizeof(Animation));
    animation->texture = texture;
    animation->width = width;
    animation->height = height;
    animation->origin = origin_value;
    animation->update_mode = update_mode;
    animation->speed = speed;
    animation->states = set;
    animation->default_state = default_state;

    cJSON_Delete(json);


    long path_length = strlen(path) + 1;
    char* animation_key = malloc(path_length);
    strcpy(animation_key, path);

    content_add(&animations, animation_key, animation);

    return animation;


parse_error:

    if(texture != NULL)
        SDL_DestroyTexture(texture);

    if(set != NULL) {
        for(int i = 0; i < set->capacity; i++) {
            if(set->cells[i].active) {
                free(set->cells[i].key);
                rect_list_free(set->cells[i].value);
                free(set->cells[i].value);
            }
        }
        animation_set_free(set);
        free(set);
    }

    cJSON_Delete(json);

    return NULL;
}

static void animation_free_resources(Animation* animation) {
    SDL_DestroyTexture(animation->texture);

    AnimationSet* set = animation->states;
    for(int i = 0; i < set->capacity; i++) {
        if(set->cells[i].active) {
            free(set->cells[i].key);
            rect_list_free(set->cells[i].value);
            free(set->cells[i].value);
        }
    }
    animation_set_free(set);
    free(set);
    free(animation);
}

void animation_free(Animation* animation) {
    for(int i = 0; i < animations.capacity; i++) {
        if(animations.cells[i].active && animations.cells[i].value == (void*)animation) {
            content_remove(&animations, animations.cells[i].key);
            free(animations.cells[i].key);
            break;
        }
    }

    animation_free_resources(animation);
}

void animation_free_all(void) {
    for(int i = 0; i < animations.capacity; i++) {
        if(animations.cells[i].active) {
            free(animations.cells[i].key);
            animation_free_resources(animations.cells[i].value);
        }
    }

    content_free(&animations);
}