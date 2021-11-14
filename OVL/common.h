#pragma once
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
#include <libapi.h>
#include <libcd.h>
#include <libds.h>
#include <libpress.h>
#include <malloc.h>
#include <kernel.h>
#include "../third_party/nugget/common/syscalls/syscalls.h"
#define printf ramsyscall_printf

#define setRGB(p, r0, g0, b0)						\
	(p)->r = r0, (p)->g = g0, (p)->b = b0

#define VMODE 0
#define SCREENXRES 320          // Screen width
#define SCREENYRES (240 + (VMODE << 4))          // Screen height : If VMODE is 0 = 240, if VMODE is 1 = 256 
#define CENTERX SCREENXRES/2    // Center of screen on x 
#define CENTERY SCREENYRES/2    // Center of screen on y
#define MARGINX 0                // margins for text display
#define MARGINY 32
#define FONTSIZE 18 * 7           // Text Field Height
#define FONTX   960
#define FONTY   0
#define OTLEN 512              // Ordering Table Length

extern DRAWENV draw[2];
extern char primbuff[2][32768];
extern char *nextpri;
extern u_long ot[2][OTLEN];
extern uint8_t db;
extern uint8_t useOT;
extern CVECTOR BGcolor;
extern int timeout;

enum OverlayNumber {
        MOTHERSHIP = -1,
        OVERLAY_HELLO = 0,
        OVERLAY_TILE = 1,
        OVERLAY_POLY = 2,
        OVERLAY_SPRT = 3,
        OVERLAY_CUBE = 4,
        OVERLAY_LIGHT = 5,
        OVERLAY_PAD = 6,
    };
extern enum OverlayNumber next_overlay;

extern void init(void);
extern void display(void);
extern void clearVRAM(void);
extern void LoadTexture(u_long * tim, TIM_IMAGE * tparam);