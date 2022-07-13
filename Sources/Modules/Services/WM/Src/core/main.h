#pragma once

#include <kot/sys.h>

typedef struct
{
    uint64_t identifier;
    uint64_t NAME;
    uint64_t framebuffer_addr;
    uint16_t framebuffer_width;
    uint16_t framebuffer_height;
    uint16_t framebuffer_pitch;
    uint16_t framebuffer_bpp;
    uint8_t  memory_model;
    uint8_t  red_mask_size;
    uint8_t  red_mask_shift;
    uint8_t  green_mask_size;
    uint8_t  green_mask_shift;
    uint8_t  blue_mask_size;
    uint8_t  blue_mask_shift;
    uint8_t  unused;
} framebuffer_t;

typedef struct
{
    uint64_t fb_addr;
    size_t fb_size;
    uint16_t width;
    uint16_t height;
    uint16_t pitch;
    uint16_t bpp;
} ScreenInfo;

extern ScreenInfo *screenInfo;