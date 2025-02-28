#ifndef _CONTEXT_UI_H_
#define _CONTEXT_UI_H_

#include <kot/types.h>

#include <kot-graphics/utils.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct {
    framebuffer_t* fb;
} ContextUi_t;

ContextUi_t* CreateUiContext(framebuffer_t* fb);

#if defined(__cplusplus)
}
#endif

#endif