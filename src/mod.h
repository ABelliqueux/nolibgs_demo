#pragma once
#include <libapi.h>
#include <libspu.h>
#include "../third_party/nugget/common/hardware/hwregs.h"
#include "../third_party/nugget/common/hardware/irq.h"
#include "../third_party/nugget/common/syscalls/syscalls.h"
#define printf ramsyscall_printf

// Mod Playback
#include "../third_party/nugget/modplayer/modplayer.h"
extern const uint8_t _binary_HIT_SHIN1_HIT_start[];
#define HITFILE _binary_HIT_SHIN1_HIT_start
extern long musicEvent;

void loadMod();
void startMusic();
void pauseMusic();
void resumeMusic();
void stopMusic();