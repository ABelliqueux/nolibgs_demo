#pragma once
#include <libapi.h>
#include <libspu.h>
#include "../../nugget/common/hardware/hwregs.h"
#include "../../nugget/common/hardware/irq.h"
#include "../../nugget/common/syscalls/syscalls.h"
#include "modplayer.h"

extern const uint8_t _binary_HIT_SHIN1_HIT_start[];
#define HITFILE _binary_HIT_SHIN1_HIT_start
#define printf ramsyscall_printf

extern long musicEvent;
void loadMod();
long processMusic();
void startMusic();
void pauseMusic();
void resumeMusic();
void stopMusic();