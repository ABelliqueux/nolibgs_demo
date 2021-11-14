#include "../common.h"

#define DBG_FNTX 36
#define DBG_FNTY 168
// Embedded TIMs
#ifdef STANDALONE
    extern unsigned long _binary_TIM_TIM16_tim_start[];
    extern unsigned long _binary_TIM_TIM8_tim_start[];
    extern unsigned long _binary_TIM_TIM4_tim_start[];
#else
    extern unsigned long _binary_OVL_hello_sprt_TIM_TIM16_tim_start[];
    extern unsigned long _binary_OVL_hello_sprt_TIM_TIM8_tim_start[];
    extern unsigned long _binary_OVL_hello_sprt_TIM_TIM4_tim_start[];
#endif
TIM_IMAGE TIM_16;
TIM_IMAGE TIM_8;
TIM_IMAGE TIM_4;

int ovl_main_sprt(void)
{   
    #ifndef STANDALONE
        useOT = 1;
    #endif
    uint16_t i = 0;
    SPRT * sprt_16b;                // Define 3 pointers to SPRT struct
    SPRT * sprt_8b;
    SPRT * sprt_4b;
    DR_TPAGE * tpage_16b;           // Define 3 pointers to DR_TPAGE struct. We need three because our images are on three 
    DR_TPAGE * tpage_8b;            // different texture pages.
    DR_TPAGE * tpage_4b;
    init();
    #ifdef STANDALONE
        LoadTexture(_binary_TIM_TIM16_tim_start, &TIM_16); // Load everything to vram
        LoadTexture(_binary_TIM_TIM8_tim_start, &TIM_8);
        LoadTexture(_binary_TIM_TIM4_tim_start, &TIM_4);
    #else
        LoadTexture(_binary_OVL_hello_sprt_TIM_TIM16_tim_start, &TIM_16); // Load everything to vram
        LoadTexture(_binary_OVL_hello_sprt_TIM_TIM8_tim_start,  &TIM_8);
        LoadTexture(_binary_OVL_hello_sprt_TIM_TIM4_tim_start,  &TIM_4);
    #endif
    while (1)
    {
        i++;
        ClearOTagR(ot[db], OTLEN);
        // Loading a 16 bit TIM 
        sprt_16b = (SPRT *)nextpri;                 // Cast whats at nexpri as a SPRT named sprt_16b
        setSprt(sprt_16b);                          // Initialize the SPRT struct
        setRGB0(sprt_16b, 128, 128, 128);           // Set RGB color. 128,128,128 is neutral. You can color the image by adjusting these values
        setXY0(sprt_16b, 28, MARGINY);              // Set sprite position
        setWH(sprt_16b, 64, 128 );                  // Set sprite width and height
        addPrim(ot[db][OTLEN-2], sprt_16b);                  // add the sprite primitive to the ordering table
        nextpri += sizeof(SPRT);                    // increment nextpri so that it points just after sprt_16b in the primitive buffer
        // Set Texture page for the 16bit tim : 768, 0 - No CLUT
        // Note :  You need to use setDrawTPage each time you want to use a texture that's on a different texture page
        tpage_16b = (DR_TPAGE*)nextpri;
        setDrawTPage(tpage_16b, 0, 1,               // Set the Texture Page the texture we want resides on.
            getTPage(TIM_16.mode&0x3, 0,            // Here we are using bitmasking to deduce the picture mode : &0x3
            TIM_16.prect->x, TIM_16.prect->y));     // In binary, 3 is 11, so we only keep the first two bits
                                                    // Values can be 00 (0), 01 (1), 10(2), respectively, 4bpp, 8bpp, 15bpp, 24bpp. See Fileformat47.pdf, p.180
                                                    // Similarly, we could use bitmasking to deduce if there is a CLUT by bitmasking the 4th bit : if(TIM_IMAGE.mode & 0x8) LoadImage... :  
        addPrim(ot[db][OTLEN-2], tpage_16b);                 // add the sprite primitive to the ordering table
        nextpri += sizeof(DR_TPAGE);                // Advance next primitive address
        // Loading a 8 bit TIM
        sprt_8b = (SPRT *)nextpri;
        setSprt(sprt_8b);
        setRGB0(sprt_8b, 128, 128, 128);        
        setXY0(sprt_8b, sprt_16b->x0 + sprt_16b->w + 32, MARGINY);
        setWH(sprt_8b, 64, 128 );
        setClut(sprt_8b, TIM_8.crect->x, TIM_8.crect->y);       // Only difference here is we set the CLUT to the position of the VRAM we loaded the palette earlier (see LoadTexture())
        addPrim(ot[db][OTLEN-2], sprt_8b);
        nextpri += sizeof(SPRT);
        // Set Texture page for the 8bit tim : 512, 256 - CLUT is at 0, 480
        tpage_8b = (DR_TPAGE*)nextpri;
        setDrawTPage(tpage_8b, 0, 1,               
            getTPage(TIM_8.mode&0x3, 0,            
            TIM_8.prect->x, TIM_8.prect->y));
        addPrim(ot[db][OTLEN-2], tpage_8b);                 
        nextpri += sizeof(DR_TPAGE);          
        // Loading a 4 bit TIM
        sprt_4b = (SPRT *)nextpri;
        setSprt(sprt_4b);
        setRGB0(sprt_4b, 128, 128, 128);        
        setXY0(sprt_4b, sprt_8b->x0 + sprt_8b->w + 32, MARGINY);
        setWH(sprt_4b, 64, 128 );
        setClut(sprt_4b, TIM_4.crect->x, TIM_4.crect->y);
        addPrim(ot[db][OTLEN-2], sprt_4b);
        nextpri += sizeof(SPRT);
        // Set Texture page for the 4bit tim : 512, 256 - CLUT is at 0, 480
        tpage_4b = (DR_TPAGE*)nextpri;
        setDrawTPage(tpage_4b, 0, 1,                
            getTPage(TIM_4.mode&0x3, 0,             
            TIM_4.prect->x, TIM_4.prect->y));
        addPrim(ot[db][OTLEN-2], tpage_4b);
        nextpri += sizeof(DR_TPAGE);
        
        FntPrint(0, "16 Bit!     ");
        FntPrint(0, "8 Bit!      ");
        FntPrint(0, "4 Bit!\n\n");
        FntPrint(0, "Check VRAM in emu to see the dif");
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