#ifndef COLOR_H
#define COLOR_H

#define MAX_BRIGHTNESS  8
#define MAX_RGB         255
#define MIN_RGB         0
#define MAX_COLOR       25

typedef struct color_t {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t w;
} Color;

typedef bool color_dir;

typedef struct pers_color_t {
    Color     color;
    color_dir dir;
} Pers_color;

typedef struct animation_t {
    Color    color[MAX_COLOR];
    uint8_t  count;
    uint8_t  increment;
    uint16_t delay;
} Animation;

void clear_animation(Animation *anim)
{
    anim->count = 0;
    anim->increment = 1;
    anim->delay = 0;
}

Color* get_first_color(Animation *anim)
{
    return &(anim->color[0]);
}

void set_first_color(Animation *anim, uint8_t r, uint8_t g, uint8_t b, uint8_t w = MAX_BRIGHTNESS)
{
    get_first_color(anim)->r = r;
    get_first_color(anim)->g = g;
    get_first_color(anim)->b = b;
    get_first_color(anim)->w = w;

    anim->count = 1;
}

void set_next_color(Animation *anim, uint8_t r, uint8_t g, uint8_t b, uint8_t w = MAX_BRIGHTNESS)
{
    // Set the color
    anim->color[anim->count].r = r;
    anim->color[anim->count].g = g;
    anim->color[anim->count].b = b;
    anim->color[anim->count].w = w;

    // Increment the count
    anim->count += 1;
}

Color new_color(uint8_t r, uint8_t g, uint8_t b, uint8_t w = MAX_BRIGHTNESS)
{
    return Color{r, g, b, w};
}

#endif //_COLOR_H
