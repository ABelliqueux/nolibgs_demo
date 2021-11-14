// The nolibgs 2021 demo disc !
#include "OVL/common.h"
// str playback
#include "src/str.h"
#include "src/mod.h"
#include "third_party/nugget/common/syscalls/syscalls.h"
#define printf ramsyscall_printf
#define DBG_FNTX 32
#define DBG_FNTY 200
//#define DEBUG
DISPENV disp[2];                 // Double buffered DISPENV and DRAWENV
DRAWENV draw[2];
char primbuff[2][32768];         // double primitive buffer of length 32768 * 8 =  262.144 bits / 32,768 Kbytes
u_long ot[2][OTLEN];             // double ordering table of length 8 * 32 = 256 bits / 32 bytes
char * nextpri = primbuff[0];     // pointer to the next primitive in primbuff. Initially, points to the first bit of primbuff[0]
uint8_t db = 1;                    // index of which buffer is used, values 0, 1
RECT tw = {0,0,0,0};
// STR playback
STR menu[4] = {
    { "\\MENU.STR;1", 256, 240, 30, 0, 0 },
    { "\\MENU.STR;1", 256, 240, 30, 1, 0 },
    { "\\MENU1.STR;1", 256, 240, 30, 0, 0 },
    { "\\MENU1.STR;1", 256, 240, 30, 1, 0 },
};
STR * curStr;
StHEADER * sectorHeader;
int8_t drawMenu = 0;
u_long * nextFrame = 0;
// Ring buffer frame address
u_long * frameAddr = 0;

// OVERLAYS
extern u_long load_all_overlays_here;

typedef struct OVERLAY {
  char filename[0x7c];
  int (*main)();
  char commandline[0x180];
  char title[0xc];
  CVECTOR BGcolor;
} OVERLAY;

int ovl_main_hello();
int ovl_main_tile();
int ovl_main_poly();
int ovl_main_sprt();
int ovl_main_cubetex();
int ovl_main_light();
int ovl_main_pad();

OVERLAY menu_items[] = {
    {"\\HELLO.OVL;1", ovl_main_hello, "",   "HELLO WORLD!", { 225, 220, 40 } },
    {"\\TILE.OVL;1",  ovl_main_tile, "",    "HELLO TILE! ", { 150, 30 , 40 } },
    {"\\POLY.OVL;1",  ovl_main_poly, "",    "HELLO POLY! ", { 60 , 127, 0  } },
    {"\\SPRT.OVL;1",  ovl_main_sprt, "",    "HELLO SPRT! ", { 0  , 108, 255 } },
    {"\\CUBTX.OVL;1", ovl_main_cubetex, "", "HELLO CUBE! ", { 255  , 200, 0 } },
    {"\\CUBLT.OVL;1", ovl_main_light, "",   "HELLO LIGHT!", { 16 , 90 , 110 } },
    {"\\PAD.OVL;1",   ovl_main_pad, "",     "HELLO PAD!  ", { 100, 40 , 140 } },
};
int curItem = 0;
#define MAX_ITEMS 6
int timeout = 200;

enum OverlayNumber next_overlay = MOTHERSHIP;
enum OverlayNumber prev_overlay = MOTHERSHIP;

CVECTOR BGcolor, MScolor = { 24, 108, 76 };
uint8_t Xpressed = 0;

// FONT COLOR
CVECTOR fntColor = { 115, 215, 45 };
CVECTOR fntColorBG = { 0, 0, 0 };

uint8_t useOT = 0;
int CDreadOK = 0;

// FUNCTIONS
void LoadTexture(u_long * tim, TIM_IMAGE * tparam);
void FntColor(CVECTOR fgcol, CVECTOR bgcol );
void init(void);
void display(void);
void drawBG(void);
void checkPad(void);
void clearVRAM(void);
int loadOverlayAndStart(OVERLAY *);
void EmptyOTag(u_long ot[2][OTLEN]);
void EmptyPrimBuf(char primbuff[2][32768], char ** nextpri);
void preInitOvl(OVERLAY * overlay);
void postInitOvl(OVERLAY * overlay);

void (*dispp) (void);

void LoadTexture(u_long * tim, TIM_IMAGE * tparam){   
        OpenTIM(tim);                                 
        ReadTIM(tparam);                              
        LoadImage(tparam->prect, tparam->paddr);      
        DrawSync(0);                                  
        if (tparam->mode & 0x8){ // check 4th bit     
            LoadImage(tparam->crect, tparam->caddr);  
            DrawSync(0);                              
    }
}

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
    SetDispMask(1);                 // Display on screen    
    setRGB0(&draw[0], BGcolor.r, BGcolor.g, BGcolor.b); // set color for first draw area
    setRGB0(&draw[1], BGcolor.r, BGcolor.g, BGcolor.b); // set color for second draw area
    draw[0].isbg = 1;               // set mask for draw areas. 1 means repainting the area with the RGB color each frame 
    draw[1].isbg = 1;
    draw[0].dfe = 0;               // set mask for draw areas. 1 means repainting the area with the RGB color each frame 
    draw[1].dfe = 0;
    draw[0].dtd = 1;
    draw[1].dtd = 1;
    PutDispEnv(&disp[db]);          // set the disp and draw environnments
    PutDrawEnv(&draw[db]);
    FntLoad(FONTX, FONTY);                // Load font to vram at 960,0(+128)
    // Debug text
    FntOpen(DBG_FNTX, DBG_FNTY, 256, 120, 0, 220 ); // FntOpen(x, y, width, height,  black_bg, max. nbr. chars
    // Menu text
    FntOpen(106, 166, 48, 20, 0, 12 ); // FntOpen(x, y, width, height,  black_bg, max. nbr. chars
    // Change font color
    FntColor(fntColor, fntColorBG);
}
void display(void)
{
    DrawSync(0);                    // Wait for all drawing to terminate
    VSync(0);                       // Wait for the next vertical blank
    PutDispEnv(&disp[db]);          // set alternate disp and draw environnments
    PutDrawEnv(&draw[db]);  
    if (useOT) { DrawOTag(&ot[db][OTLEN - 1]); }
    db = !db;
    if (useOT) { nextpri = primbuff[db];  }               // flip db value (0 or 1)
}
void drawBG(void)
{
    POLY_FT4 * poly = 0;                           // pointer to a POLY_G4     
    ClearOTagR(ot[db], OTLEN);
    
    poly = (POLY_FT4 *)nextpri;                    // Set poly to point to  the address of the next primitiv in the buffer
    // Set transform matrices for this polygon
    setPolyFT4(poly);                             // Initialize poly as a POLY_F4 
    poly->tpage = getTPage(2,0,STR_POS_X, STR_POS_Y);
    setRGB0(poly, 128, 128, 128);                 // Set poly color (neutral here)
    setXY4(poly, 
            0, 0,
            0, SCREENYRES,
            SCREENXRES, 0,
            SCREENXRES, SCREENYRES);
    setUV4(poly, 0, 0,
                 0, 240,
                 255, 0,
                 255, 240);                     // Set UV coordinates in order Top Left, Bottom Left, Top Right, Bottom Right 
    addPrim(ot[db][OTLEN-1], poly);                      // add poly to the Ordering table        
}
void clearVRAM(void)
{
    RECT vram = {0,0,1024,512};
    ClearImage(&vram,0,0,0);
}

void preInitOvl(OVERLAY * overlay)
{
    //~ clearVRAM();
    ResetCallback();
    ResetGraph(3);
    EmptyPrimBuf(primbuff, &nextpri);
    EmptyOTag(&ot[db]);
    setRGB(&BGcolor, overlay->BGcolor.r, overlay->BGcolor.g, overlay->BGcolor.b );
}
void postInitOvl(OVERLAY * overlay)
{
    clearVRAM();
    //~ ResetGraph(3);
    EmptyPrimBuf(primbuff, &nextpri);
    EmptyOTag(&ot[db]);
    setRGB(&BGcolor, MScolor.r, MScolor.g, MScolor.b);
    setRGB0(&draw[0], BGcolor.r, BGcolor.g, BGcolor.b); // set color for first draw area
    setRGB0(&draw[1], BGcolor.r, BGcolor.g, BGcolor.b);
    FntLoad(FONTX, FONTY);                // Load font to vram at 960,0(+128)
    FntOpen(DBG_FNTX, DBG_FNTY, 256, 120, 0, 220 ); // FntOpen(x, y, width, height,  black_bg, max. nbr. chars
    // Menu text
    FntOpen(106, 166, 48, 20, 0, 12); // FntOpen(x, y, width, height,  black_bg, max. nbr. chars
    // Change font color
    FntColor(fntColor, fntColorBG);
}
int loadOverlayAndStart(OVERLAY * overlay)
{
    int CDreadResult = 0;
    int next_ovl = -1;
    // Load overlay file from CD
    CDreadResult = CdReadFile(overlay->filename, &load_all_overlays_here, 0);
	CdReadSync(0, 0);
    printf(0,"CD read: %d", CDreadResult);
    // If file loaded sucessfully
    if (CDreadResult)
    {
        // Pre OVL
        preInitOvl(overlay);
        // Exec OVL
        next_ovl = overlay->main();
        // Post OVL
        postInitOvl(overlay);
    }
    return next_ovl;
}
//~ void EmptyOTag(u_long ot[2][OTLEN], char * primbuff, char ** nextpri )
void EmptyPrimBuf(char primbuff[2][32768], char ** nextpri)
{
    for(uint16_t p; p < 32768; p++)
    {
        primbuff[0][p] = 0;
        primbuff[1][p] = 0;
    }
    *nextpri = primbuff[0];
}
void EmptyOTag(u_long ot[2][OTLEN])
{
    for (uint16_t p; p < OTLEN; p++)
    {
        ot[0][p] = 0;
        ot[1][p] = 0;
    }
}
void checkPad(void)
{
    u_long pad = 0;
    static u_long oldPad;    
    pad = PadRead(0);
    
    if (drawMenu){
        if ( pad & PADLleft && !(oldPad & PADLleft) )
        {
            // Channel 1 is transition anim, only take input when !transition
            if ( curStr == &menu[IDLE] )
            {
                ramsyscall_printf("Left\n");
                // Switch sound
                MOD_PlayNote(22, 12, 15, 63);
                switchStr(&curStr, LEFT);
                if ( curItem < MAX_ITEMS ){
                    curItem++;
                } else {
                    curItem = 0;
                }
                ramsyscall_printf("curItem: %d\n", curItem);
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
                if ( curItem > 0 ){
                    curItem--;
                } else {
                    curItem = MAX_ITEMS;
                }
                ramsyscall_printf("curItem: %d\n", curItem);
            }
            oldPad = pad;
        }
        if ( !(pad & PADLright) && oldPad & PADLright )
        {
            oldPad = pad;
        }
    }
        // Cross button
        if ( pad & PADRdown && !(oldPad & PADRdown) )
        {
            Xpressed = 1;
            oldPad = pad;
        }
        if ( !(pad & PADRdown) && oldPad & PADRdown )
        {
            Xpressed = 0;
            oldPad = pad;
        }
        if ( pad & PADstart && !(oldPad & PADstart) )
        {
            drawMenu = !drawMenu;
            oldPad = pad;
        }
        if ( !(pad & PADstart) && oldPad & PADstart )
        {
            oldPad = pad;
        }
    
}
int main() {
    int t = 0;
    // STR
    curStr = &(menu[0]);
    // OVL
    prev_overlay = next_overlay; 
    // Set bg color
    setRGB(&BGcolor, MScolor.r, MScolor.g, MScolor.b);
    // Mute spu to get rid of startup sound
    spuMute();
    // Init display
    init();
    // Init input
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
        if (drawMenu = 1)
        {
            useOT = 1;
            // Draw menu prim once
            drawBG();
            drawMenu = -1;

        }
        // While end of str is not reached, play it
        if (curStr->endPlayback == 1 && next_overlay == MOTHERSHIP )
        {   
            // Replay STR
            resetSTR(curStr);
        }
        if ( curStr->endPlayback == 0 )
        {   
            playSTR(&curStr);
            if ( curStr->channel != 1 && drawMenu )
            {
                // Display title
                FntPrint(1, "%s", menu_items[curItem].title);
                // Flickering text
                if ( sectorHeader->frameCount > 5 )
                {
                    FntFlush(1);
                } else if ( (sectorHeader->frameCount % 2) && sectorHeader->frameCount < 5 )
                {
                    FntFlush(1);
                }
                // If the cross button is pressed
                if( Xpressed ) {
                    // If we're on the menu, animate menu
                    if ( next_overlay == MOTHERSHIP ){
                        // Select sound
                        MOD_PlaySoundEffect(22, 7, 15, 63);
                        drawMenu = 0;
                        curStr->endPlayback = 1;
                        stopSTR(curStr);
                        useOT = 0;
                        next_overlay = curItem;
                    }
                }
            }
        }
        // If next overlay is not current overlay, load it
        if (next_overlay != MOTHERSHIP ){
            prev_overlay = next_overlay;
            next_overlay = loadOverlayAndStart(&menu_items[next_overlay]);
            t = 0;
        }
        t++;
        #ifdef DEBUG
            FntPrint(0, "Mothership:  %d\nOvl: %d %d %d\n%x %x %x\nstr x: %d", t, prev_overlay, next_overlay, curItem, nextpri, primbuff[0], primbuff[1], curStr->x);
            FntFlush(0);
        #endif
        display();
    }
    return 0;
}

