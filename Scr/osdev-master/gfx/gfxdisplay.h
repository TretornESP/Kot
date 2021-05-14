// ------------------------------------------------------------------------------------------------
// gfx/gfxdisplay.h
//
// VGA and Mode Change Function
// ------------------------------------------------------------------------------------------------

#pragma once

#include "gfx/gfxpci.h"

// ------------------------------------------------------------------------------------------------
typedef struct GfxDisplay
{
    // MWDD FIX: TO DO
    int dummy;
} GfxDisplay;

// ------------------------------------------------------------------------------------------------
void GfxInitDisplay(GfxDisplay *display);
void GfxDisableVga(GfxPci *pci);
