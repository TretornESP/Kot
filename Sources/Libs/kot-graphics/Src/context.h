#pragma once

#include <kot/math.h>
#include <kot/utils/map.h>
#include <kot/cstring.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    uint32_t x;
    uint32_t y;
} pos_t;

typedef struct {
    uintptr_t fb_addr;
    size64_t fb_size;
    uint32_t width;
    uint32_t height;
    uint32_t pitch;
    uint8_t bpp;
    uint8_t btpp; // bpp / 8
    bool _auto;
    bool _scaling;
    uint32_t x;
    uint32_t y;
    vector_t* poses;
    uint16_t scale;
} ctxg_t;

ctxg_t* CreateGraphicContext(uintptr_t fb_addr, uint32_t width, uint32_t height);

void ctxPutPixel(ctxg_t* ctx, uint32_t x, uint32_t y, uint32_t colour);
int8_t ctxPixelExist(ctxg_t* ctx, uint32_t x, uint32_t y);
uint32_t ctxPetPixel(ctxg_t* ctx, uint32_t x, uint32_t y);

void ctxFillRect(ctxg_t* ctx, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour);
void ctxDrawRect(ctxg_t* ctx, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t colour);
void ctxDrawLine(ctxg_t* ctx, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t colour);

void ctxDrawCircle(ctxg_t* ctx, uint32_t xc, uint32_t yc, uint32_t radius, uint32_t colour);

void ctxFill(ctxg_t* ctx, uint32_t x, uint32_t y, uint32_t colour, uint32_t border);

// path function

void draw(ctxg_t* ctx, uint32_t colour);

void abs_pos(ctxg_t* ctx, uint32_t x, uint32_t y);
void rel_pos(ctxg_t* ctx, uint32_t x, uint32_t y);

void add_pos(ctxg_t* ctx);
void end_path(ctxg_t* ctx);
void reset(ctxg_t* ctx);

pos_t* get_pos(ctxg_t* ctx, uint16_t index);

void auto_pos(ctxg_t* ctx, bool _auto);
void scale_pos(ctxg_t* ctx, bool _scaling);

uint16_t get_scale(ctxg_t* ctx);

// framebuffer function

void swapTo(ctxg_t* ctx, uintptr_t to);
void swapFrom(ctxg_t* ctx, uintptr_t from);
void swapToCtx(ctxg_t* ctx);
void swapFromCtx(ctxg_t* ctx);

void clear(ctxg_t* ctx);
void clearColor(ctxg_t* ctx, uint32_t colour);

uintptr_t getFramebuffer(ctxg_t* ctx);

void blendAlpha(uintptr_t Pixel, uint32_t color);

/* uint32_t getPitch(ctxg_t* ctx) {
    return ctx->pitch;
} */

#if defined(__cplusplus)
}
#endif