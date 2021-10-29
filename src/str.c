#include "str.h"

CdlFILE STRfile;

// Parameter we want to set the CDROM with
u_char param = CdlModeSpeed;
// Buffers total ~110KB in memory 
// SECTOR_SIZE is defined in words : 512 * 4 == 2048 Bytes/sector
// Ring buffer : 32 * 2048 = 65536 Bytes
u_long  RingBuff[ RING_SIZE * SECTOR_SIZE ];
// VLC buffers : display area in words (hence /2), 160*320 == 38400 Bytes
u_long  VlcBuff[ SCREENXRES / 2 * SCREENYRES ];  
// If using 16bpp, fetch 16xYres strips, if 24bpp, fetch 24xYres strips, 5120*PPW Bytes 
u_short ImgBuff[ 16 * SCREENYRES];

u_long * curVLCptr = &VlcBuff[0];
u_short * curIMGptr = &ImgBuff[0];

void loadCdFile(STR * str)
{
    if ( CdSearchFile(&STRfile, str->name) == 0 ) {
        FntPrint("File not found :%s\n", str->name);
    }
    CdControl(CdlSetloc, (u_char *)&STRfile.pos, 0);
    //CdControl(CdlSetmode, &param, 0);
    CdRead2(CdlModeStream|CdlModeSpeed);
}

void initSTR(STR * str)
{
    DecDCTReset(0);
    StSetRing(RingBuff, RING_SIZE);
    StSetStream(0, 1, str->length, 0, 0);
    // Set CD mode to CdlModeSpeed
    CdControl(CdlSetmode, &param, 0);
    loadCdFile(str);
}
void resetSTR(STR * str)
{
    u_long * curVLCptr = &VlcBuff[0];
    u_short * curIMGptr = &ImgBuff[0];
    loadCdFile(str);
    //~ // Set the seek target position
    //~ CdControl(CdlSetloc, (u_char *)&STRfile.pos, 0);
    //~ // Read from CD at position &STRfile.pos
    //~ // Enable streaming, double speed 
    //~ CdRead2(CdlModeStream|CdlModeSpeed);
    str->endPlayback = 0;
}

void switchStr(STR ** str, int strID)
{
    // Switch current STR
    printf("p0: %p - %d - ", *str, (*str)->channel);
    sectorHeader->frameCount = 0;
    *str = &menu[strID];
    printf("p1: %p\n", *str);
    StSetChannel( (*str)->channel );
    (*str)->endPlayback = 1;
}
void playSTR(STR ** str)
{
    //~ printf("Frame %d / %d,ch: %d, p: %p, m0: %p, m1: %p\n", sectorHeader->frameCount, (*str)->length, (*str)->channel, str, &menu[0], &menu[1]);
    // Use this area to draw the slices
    RECT curSlice = { STR_POS_X, 
                      STR_POS_Y,
                      16,
                      (*str)->y};
    int frameDone = 0;
    // Reset counter
    int wait = WAIT_TIME;
    // Dont try decoding if not data has been loaded from ring buffer
    if ( frameAddr ){
        // Begin decoding RLE-encoded MDEC image data
        DecDCTin( curVLCptr , DCT_MODE);
        // Prepare to receive the decoded image data from the MDEC
        while (curSlice.x < ( STR_POS_X + (*str)->x ) )
        {
            // Receive decoded data : a 16*ppw*240 px slice 
            DecDCTout( (u_long *) curIMGptr, curSlice.w * curSlice.h / 2);
            // Wait for transfer end
            DecDCToutSync(1);
            // Transfer data from main memory to VRAM
            LoadImage(&curSlice, (u_long *) curIMGptr );
            // Increment drawArea's X with slice width (16 or 24 pix)
            curSlice.x += 16;
        }
         // Set frameDone flag to 1
        frameDone = 1;

        curSlice.x = STR_POS_X;
        curSlice.y = STR_POS_Y;
    }
    // Get one frame of ring buffer data
    // StGetNext is non-blocking, so we wait for it to return 0.
    // StGetNext will lock the region at &frameAddr until StFreeRing() is called.
    while ( StGetNext((u_long **)&frameAddr,(u_long **)&sectorHeader) ) 
    {
        wait--;
        if (wait == 0)
            break;
    }

    // Grab a frame from the stream
    wait = WAIT_TIME;
    while ((nextFrame = frameAddr) == 0) 
    {
        wait--;
        if ( wait == 0 ){ 
            break;
        }
    }
    // Decode the Huffman/VLC compressed data
    DecDCTvlc(nextFrame, curVLCptr);
    // Unlock area obtained by StGetNext()
    StFreeRing(nextFrame);
    // Reset counter
    wait = WAIT_TIME;
    // Wait until the whole frame is loaded to VRAM
    while ( frameDone == 0 ) 
    {
        wait--;
        if ( wait == 0 ) { 
            // If a timeout occurs, force switching buffers
            frameDone = 1;
            curSlice.x = STR_POS_X;
            curSlice.y = STR_POS_Y;
        }
    }
    
   // If the current frame's number is bigger than the number of frames in STR,
    // set the endPlayback flag.
    if ( sectorHeader->frameCount >= ((*str)->length - 1) )
    {
        // If on channel 1, go back to channel 0 after 30 frames
        if ((*str)->channel)
        {
            switchStr(str, IDLE);
        } else {
        // if on channel 0, set end flag for loop
            (*str)->endPlayback = 1;
        }
        if (!drawMenu)
        {
            drawMenu = 1;
        }
    }
}