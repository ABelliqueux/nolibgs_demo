#include "../common.h"

int ovl_main_pad()
{
    #ifndef STANDALONE
        useOT = 1;
    #endif
    uint16_t i = 0;
    TILE * PADL;                    // Tile primitives
    TILE * TRIGGERL;
    TILE * PADR;
    TILE * TRIGGERR;
    TILE * START, * SELECT;
    int pad = 0;
    init();
    while (1)
    {
        i++;
        ClearOTagR(ot[db], OTLEN);
        // D-cross
        PADL = (TILE *)nextpri;
        setTile(PADL);
        setRGB0(PADL, 0, 0, 255);        
        setXY0(PADL, CENTERX - 80, CENTERY);
        setWH(PADL, 24, 24);
        addPrim(ot[db], PADL);
        nextpri += sizeof(TILE);
        // L1+L2
        TRIGGERL = (TILE *)nextpri;
        setTile(TRIGGERL);
        setRGB0(TRIGGERL, 255, 0, 0);        
        setXY0(TRIGGERL, CENTERX - 80, CENTERY - 80);
        setWH(TRIGGERL, 24, 24);
        addPrim(ot[db], TRIGGERL);
        nextpri += sizeof(TILE);
        // /\, X, O, [] 
        PADR = (TILE *)nextpri;
        setTile(PADR);
        setRGB0(PADR, 0, 255, 0);        
        setXY0(PADR, CENTERX + 50, CENTERY);
        setWH(PADR, 24, 24);
        addPrim(ot[db], PADR);
        nextpri += sizeof(TILE);
        // R1+R2
        TRIGGERR = (TILE *)nextpri;
        setTile(TRIGGERR);
        setRGB0(TRIGGERR, 255, 0, 255);        
        setXY0(TRIGGERR, CENTERX + 50, CENTERY -80);
        setWH(TRIGGERR, 24, 24);
        addPrim(ot[db], TRIGGERR);
        nextpri += sizeof(TILE);
        // START + SELECT
        START = (TILE *)nextpri;
        setTile(START);
        setRGB0(START, 240, 240, 240);        
        setXY0(START, CENTERX - 16, CENTERY - 36);
        setWH(START, 24, 24);
        addPrim(ot[db], START);
        nextpri += sizeof(TILE);
        // Pad stuff
        pad = PadRead(0);                             // Read pads input. id is unused, always 0.
                                                      // PadRead() returns a 32 bit value, where input from pad 1 is stored in the low 2 bytes and input from pad 2 is stored in the high 2 bytes. (https://matiaslavik.wordpress.com/2015/02/13/diving-into-psx-development/)
        // D-pad        
        if(pad & PADLup)   {PADL->y0 = CENTERY - 16;} // 🡩           // To access pad 2, use ( pad >> 16 & PADLup)...
        if(pad & PADLdown) {PADL->y0 = CENTERY + 16;} // 🡫
        if(pad & PADLright){PADL->x0 = CENTERX - 64;} // 🡪
        if(pad & PADLleft) {PADL->x0 = CENTERX - 96;} // 🡨
        // Buttons
        if(pad & PADRup)   {PADR->y0 = CENTERY - 16;} //   △
        if(pad & PADRdown) {PADR->y0 = CENTERY + 16;} //   ╳
        if(pad & PADRright){PADR->x0 = CENTERX + 66;} //   ⭘
        if(pad & PADRleft) {PADR->x0 = CENTERX + 34;} //   ⬜
        // Shoulder buttons
        if(pad & PADL1){TRIGGERL->y0 = CENTERY - 64;} // L1
        if(pad & PADL2){TRIGGERL->y0 = CENTERY - 96;} // L2
        if(pad & PADR1){TRIGGERR->y0 = CENTERY - 64;} // R1
        if(pad & PADR2){TRIGGERR->y0 = CENTERY - 96;} // R2
        // Start & Select
        if(pad & PADstart){START->w = 32; START->h = 32;START->x0 -= 4;START->y0 -= 4;} // START
        if(pad & PADselect){START->r0 = 0;}                                             // SELECT
        FntPrint(0, "Hello Pad! %d", i);
        FntFlush(0);
        #ifndef STANDALONE
            if (i == timeout){
                useOT = 0;
                next_overlay = MOTHERSHIP;
                break;
            }
        #endif
        display();
        
    } 
    return next_overlay;
}
