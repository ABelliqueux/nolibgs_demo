// The nolibgs 2021 demo disc !
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <libgte.h>
#include <libetc.h>
#include <libgpu.h>
// CD library
#include <libcd.h>
// CODEC library
#include <libpress.h>
// str playback
#include "src/str.h"
#include "src/mod.h"

#define VMODE 0                 // Video Mode : 0 : NTSC, 1: PAL
#define SCREENXRES 320          // Screen width
#define SCREENYRES 240           // Screen height : If VMODE is 0 = 240, if VMODE is 1 = 256 
#define CENTERX SCREENXRES/2    // Center of screen on x 
#define CENTERY SCREENYRES/2    // Center of screen on y
#define FONTX   960
#define FONTY   0
#define OTLEN 8 
DISPENV disp[2];                 // Double buffered DISPENV and DRAWENV
DRAWENV draw[2];
u_long ot[2][OTLEN];               // double ordering table of length 8 * 32 = 256 bits / 32 bytes
char primbuff[2][32768];     // double primitive buffer of length 32768 * 8 =  262.144 bits / 32,768 Kbytes
char *nextpri = primbuff[0];       // pointer to the next primitive in primbuff. Initially, points to the first bit of primbuff[0]
short db = 1;                    // index of which buffer is used, values 0, 1

STR menu[4] = {
    { "\\MENU.STR;1", 256, 240, 30, 0, 0 },
    { "\\MENU.STR;1", 256, 240, 30, 1, 0 },
    { "\\MENU1.STR;1", 256, 240, 30, 0, 0 },
    { "\\MENU1.STR;1", 256, 240, 30, 1, 0 },
};
STR * curStr;
StHEADER * sectorHeader;
uint8_t drawMenu = 0;
u_long * nextFrame = 0;
// Ring buffer frame address
u_long * frameAddr = 0;

void init(void);
void FntColor(CVECTOR fgcol, CVECTOR bgcol );
void display(void);
void drawBG(void);
void checkPad(void);

typedef struct OVERLAY {
  char filename[0x7c];
  int (*main)();
  char commandline[0x180];
  char title[0xc];
} OVERLAY;

OVERLAY menu_items[1] = {
    {"\\HELLO.OVL;1", 0, "", "0123456789AB"}
};

CVECTOR fntColor = { 115, 215, 45 };
CVECTOR fntColorBG = { 0, 0, 0 };
void FntColor(CVECTOR fgcol, CVECTOR bgcol )
{
    // The debug font clut is at tx, ty + 128
    // tx = bg color
    // tx + 1 = fg color
    // We can override the color by drawing a rect at these coordinates
    // 
    RECT fg = { FONTX+1, FONTY + 128, 1, 1 };
    RECT bg = { FONTX, FONTY + 128, 1, 1 };
    ClearImage(&fg, fgcol.r, fgcol.g, fgcol.b);
    ClearImage(&bg, bgcol.r, bgcol.g, bgcol.b);
    
}

void init(void)
{
    ResetCallback();
    ResetGraph(0);                 // Initialize drawing engine with a complete reset (0)
    InitGeom();
    SetGeomOffset(CENTERX,CENTERY);
    SetGeomScreen(CENTERX);
    SetDefDispEnv(&disp[0], 0, 0         , SCREENXRES, SCREENYRES);     // Set display area for both &disp[0] and &disp[1]
    SetDefDispEnv(&disp[1], 0, SCREENYRES, SCREENXRES, SCREENYRES);     // &disp[0] is on top  of &disp[1]
    SetDefDrawEnv(&draw[0], 0, SCREENYRES, SCREENXRES, SCREENYRES);     // Set draw for both &draw[0] and &draw[1]
    SetDefDrawEnv(&draw[1], 0, 0         , SCREENXRES, SCREENYRES);     // &draw[0] is below &draw[1]
    // Set video mode
    #if VMODE
        SetVideoMode(MODE_PAL);
        disp[0].disp.y = 8;
        disp[1].disp.y = 8;
    #endif
    SetDispMask(0);                 // Display on screen    
    setRGB0(&draw[0], 0, 0, 0); // set color for first draw area
    setRGB0(&draw[1], 0, 0, 0); // set color for second draw area
    draw[0].isbg = 1;               // set mask for draw areas. 1 means repainting the area with the RGB color each frame 
    draw[1].isbg = 1;
    PutDispEnv(&disp[db]);          // set the disp and draw environnments
    PutDrawEnv(&draw[db]);
    FntLoad(FONTX, FONTY);                // Load font to vram at 960,0(+128)
    FntOpen(106, 166, 48, 20, 0, 12 ); // FntOpen(x, y, width, height,  black_bg, max. nbr. chars
    FntColor(fntColor, fntColorBG);
}
void display(void)
{
    DrawSync(0);                    // Wait for all drawing to terminate
    VSync(0);                       // Wait for the next vertical blank
    //~ checkMusic();
    PutDispEnv(&disp[db]);          // set alternate disp and draw environnments
    PutDrawEnv(&draw[db]);  
    DrawOTag(&ot[db][OTLEN - 1]);
    db = !db;
    nextpri = primbuff[db];                 // flip db value (0 or 1)
}
void drawBG(void)
{
    POLY_FT4 *poly = {0};                           // pointer to a POLY_G4 
    SVECTOR RotVector = {0, 0, 0};                  // Initialize rotation vector {x, y, z}
    VECTOR  MovVector = {0, 0, CENTERX/2, 0};               // Initialize translation vector {x, y, z, pad}
    
    SVECTOR VertPos[4] = {                          // Set initial vertices position relative to 0,0 - see here : https://psx.arthus.net/docs/poly_f4.jpg
            {-CENTERX/2, -CENTERY/2, 1 },                         // Vert 1 
            {-CENTERX/2,  CENTERY/2, 1 },                         // Vert 2
            { CENTERX/2, -CENTERY/2, 1 },                         // Vert 3
            { CENTERX/2,  CENTERY/2, 1  }                         // Vert 4
        };                                          
    MATRIX PolyMatrix = {0};                   
    long polydepth;
    long polyflag;
    ClearOTagR(ot[db], OTLEN);
    poly = (POLY_FT4 *)nextpri;                    // Set poly to point to  the address of the next primitiv in the buffer
    // Set transform matrices for this polygon
    RotMatrix(&RotVector, &PolyMatrix);           // Apply rotation matrix
    TransMatrix(&PolyMatrix, &MovVector);         // Apply translation matrix   
    SetRotMatrix(&PolyMatrix);                    // Set default rotation matrix
    SetTransMatrix(&PolyMatrix);                  // Set default transformation matrix        
    setPolyFT4(poly);                             // Initialize poly as a POLY_F4 
    poly->tpage = getTPage(2,0,STR_POS_X, STR_POS_Y);
    setRGB0(poly, 128, 128, 128);                 // Set poly color (neutra here)
    RotTransPers4(
                &VertPos[0],      &VertPos[1],      &VertPos[2],      &VertPos[3],
                (long*)&poly->x0, (long*)&poly->x1, (long*)&poly->x2, (long*)&poly->x3,
                &polydepth,
                &polyflag
                );                                 // Perform coordinate and perspective transformation for 4 vertices
    setUV4(poly, 0, 0,
                 0, 240,
                 255, 0,
                 255, 240);  // Set UV coordinates in order Top Left, Bottom Left, Top Right, Bottom Right 
    addPrim(ot[db], poly);                         // add poly to the Ordering table        
    nextpri += sizeof(POLY_FT4);                    // increment nextpri address with size of a POLY_F4 struct 
}
void checkPad(void)
{
    u_short pad = 0;
    static u_short oldPad;    
    pad = PadRead(0);

    if ( pad & PADLleft && !(oldPad & PADLleft) )
    {
        // Channel 1 is transition anim, only take input when !transition
        if ( curStr == &menu[IDLE] )
        {
            ramsyscall_printf("Left\n");
            // Switch sound
            MOD_PlayNote(23, 12, 15, 63);
            switchStr(&curStr, LEFT);
        }
        oldPad = pad;
    }
    if ( !(pad & PADLleft) && oldPad & PADLleft )
    {
        oldPad = pad;
    }
    // Right
    if ( pad & PADLright && !(oldPad & PADLright) )
    {
        // Channel 1 is transition anim, only take input when !transition
        if( curStr == &menu[IDLE] )
        {
            ramsyscall_printf("Right\n");
            // Switch sound
            MOD_PlayNote(23, 12, 15, 63);
            switchStr(&curStr, RIGHT);
        }
        oldPad = pad;
    }
    if ( !(pad & PADLright) && oldPad & PADLright )
    {
        oldPad = pad;
    }
    // Cross button
    if ( pad & PADRdown && !(oldPad & PADRdown) )
    {
        // Select sound
        MOD_PlayNote(22, 7, 15, 63);
        oldPad = pad;
    }
    if ( !(pad & PADRdown) && oldPad & PADRdown )
    {
        oldPad = pad;
    }
}
int main() {
    curStr = &(menu[0]);
    // Set pointers to the relevant buffer addresses
    u_long * curVLCptr = &VlcBuff[0];
    u_short * curIMGptr = &ImgBuff[0];
    
    init();
    PadInit(0);
    VSyncCallback(checkPad);
    // Init CDrom system
    CdInit();
    // Init MDEC and load STR
    initSTR(curStr); 
    // Set Channel
    StSetChannel( curStr->channel );
    // Mod Playback
    loadMod();
    startMusic();
    // Main loop
    while (1) 
    {
        // Only display background STR if drawMenu is set
        if (drawMenu)
        {               
            SetDispMask(1);
            drawBG();
        }
        // While end of str is not reached, play it
        if (curStr->endPlayback == 1)
        {   
            // Replay STR
            resetSTR(curStr);
        }
        if ( curStr->endPlayback == 0)
        {   
            playSTR(&curStr);
            if ( !curStr->channel )
            {
                // Display title
                FntPrint("%s", menu_items[0].title);
                // Flickering text
                if ( sectorHeader->frameCount > 5 )
                {
                    FntFlush(-1);
                } else if ( (sectorHeader->frameCount % 2) && sectorHeader->frameCount < 5 )
                {
                    FntFlush(-1);
                }
            }
        }
        display();
    }
    return 0;
}

