#pragma once
#include "../../nugget/common/hardware/hwregs.h"
#include "../../nugget/common/hardware/irq.h"
#include "../../nugget/common/syscalls/syscalls.h"
#include "modplayer.h"

extern const uint8_t _binary_HIT_shine_hit_start[];
#define HITFILE _binary_HIT_shine_hit_start
#define printf ramsyscall_printf

extern uint16_t s_nextCounter;
void checkMusic();
void waitVSync();
void loadMod();
void playMod();
