#include <kot/uisd.h>
#include <kot/utils.h>
#include <kot/types.h>

#include <kot-graphics/utils.h> 

#ifndef __KOT__GRAPHICS__EVENT__
#define __KOT__GRAPHICS__EVENT__

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    uintptr_t Buffer;
    size64_t Size;
    uint64_t Pitch;
    uint64_t Width;
    uint64_t Height;
    uint64_t Bpp;
    uint64_t Btpp;
} graphiceventbuffer_t;

graphiceventbuffer_t* CreateEventBuffer(uint64_t Width, uint64_t Height);
void FreeEventBuffer(graphiceventbuffer_t* EventBuffer);
void SetGraphicEventbuffer(graphiceventbuffer_t* Framebuffer, uint64_t Value, uint64_t Width, uint64_t Height, uint64_t PositionX, uint64_t PositionY);
void SetGraphicEventbufferRadius(graphiceventbuffer_t* Framebuffer, uint64_t Value, uint64_t Width, uint64_t Height, uint64_t PositionX, uint64_t PositionY, uint64_t BorderRadius);
void BlitGraphicEventbuffer(graphiceventbuffer_t* To, graphiceventbuffer_t* From, uint64_t PositionX, uint64_t PositionY);
void BlitGraphicEventbufferRadius(graphiceventbuffer_t* to, graphiceventbuffer_t* from, uint64_t PositionX, uint64_t PositionY, uint64_t BorderRadius);
uint64_t GetEventData(graphiceventbuffer_t* EventBuffer, uint64_t PositionX, uint64_t PositionY);

#if defined(__cplusplus)
}
#endif

#endif