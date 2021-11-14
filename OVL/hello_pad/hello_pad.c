#include "../common.h"

DISPENV disp[2];                 
DRAWENV draw[2];
u_long ot[2][OTLEN];             
char primbuff[2][32768];         
char *nextpri = primbuff[0];     
uint8_t db  = 0;            
CVECTOR BGcolor = { 140, 160, 40 };

void init()
{
    ResetGraph(0);
    SetDefDispEnv(&disp[0], 0, 0, SCREENXRES, SCREENYRES);
    SetDefDispEnv(&disp[1], 0, SCREENYRES, SCREENXRES, SCREENYRES);
    SetDefDrawEnv(&draw[0], 0, SCREENYRES, SCREENXRES, SCREENYRES);
    SetDefDrawEnv(&draw[1], 0, 0, SCREENXRES, SCREENYRES);
    if (VMODE)
    {
        SetVideoMode(MODE_PAL);
        disp[0].screen.y += 8;
        disp[1].screen.y += 8;
        }
    SetDispMask(1);                
    setRGB0(&draw[0], BGcolor.r, BGcolor.g, BGcolor.b);
    setRGB0(&draw[1], BGcolor.r, BGcolor.g, BGcolor.b);
    draw[0].isbg = 1;
    draw[1].isbg = 1;
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    FntLoad(960, 0);
    FntOpen(MARGINX, SCREENYRES - MARGINY - FONTSIZE, SCREENXRES - MARGINX * 2, FONTSIZE, 0, 280 );
}
void display(void)
{
    DrawSync(0);
    VSync(0);
    PutDispEnv(&disp[db]);
    PutDrawEnv(&draw[db]);
    DrawOTag(&ot[db][OTLEN - 1]);
    db = !db;
    nextpri = primbuff[db];
}

#include "hello_ovl_pad.c"

int main(void)
{
    ovl_main_pad();
}
