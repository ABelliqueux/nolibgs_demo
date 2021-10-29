#pragma once
#include "../OVL/common.h"

#define STR_POS_X SCREENXRES
#define STR_POS_Y 0
// Ring Buffer size (reduce if flickering occurs)
#define RING_SIZE   32
#define PPW         1
#define DCT_MODE    2
typedef struct STR {
    char * name;
    uint16_t x,y;
    uint16_t length;
    uint8_t channel, endPlayback;
} STR;
// 2 channels STR: ch0 = idle loop, ch1 = transition anim
extern STR menu[4];
// CD File descriptor
extern CdlFILE STRfile;
extern StHEADER * sectorHeader;
// Parameter we want to set the CDROM with
extern u_char param;
// Ring buffer : 32 * 2048 = 65536 Bytes
extern u_long  RingBuff[ RING_SIZE * SECTOR_SIZE ];
// VLC buffers : display area in words (hence /2), 160*320 == 38400 Bytes
extern u_long  VlcBuff[ SCREENXRES / 2 * SCREENYRES ];  
// If using 16bpp, fetch 16xYres strips, if 24bpp, fetch 24xYres strips, 5120*PPW Bytes 
extern u_short ImgBuff[ 16 * SCREENYRES];

extern u_long * curVLCptr;
extern u_short * curIMGptr;
// Next Ring Buffer Frame address
extern u_long * nextFrame;
// Ring buffer frame address
extern u_long * frameAddr;
extern uint8_t drawMenu;

enum MENU_STATE 
{
    IDLE = 0,
    LEFT = 1,
    RIGHT = 3,
};

void loadCdFile(STR * str);
void initSTR(STR * str);
void stopSTR();
void resetSTR(STR * str);
void playSTR(STR ** str);
void switchStr(STR ** curStr, int strID);
