#pragma once
#include "../OVL/common.h"

#define STR_POS_X SCREENXRES
#define STR_POS_Y 0
// Ring Buffer size (reduce if flickering occurs)
#define RING_SIZE   32
#define DCT_MODE    2
typedef struct STR {
    char * name;
    uint16_t x,y;
    uint16_t length;
    uint8_t channel, endPlayback;
} STR;
// 2 x 2 channels STR: str0 ch0 = idle loop, str0 ch1 = left transition anim - str1 ch0 = idle loop, str1 ch1 = right transition anim
extern STR menu[4];
// CD File descriptor
extern StHEADER * sectorHeader;
// Next Ring Buffer Frame address
extern u_long * nextFrame;
// Ring buffer frame address
extern u_long * frameAddr;
extern int8_t drawMenu;

enum MENU_STATE 
{
    IDLE = 0,
    LEFT = 1,
    RIGHT = 3,
};

void initSTR(STR * str);
void stopSTR(STR * str);
void resetSTR(STR * str);
void playSTR(STR ** str);
void switchStr(STR ** curStr, int strID);
