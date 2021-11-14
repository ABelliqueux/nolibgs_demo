TARGET = nolibgs_demo

.PHONY: all cleansub
all:
	$(MAKE) -C OVL/hello_cubetex clean all
	$(MAKE) -C OVL/hello_light clean all
	$(MAKE) -C OVL/hello_ovl_world  clean all
	$(MAKE) -C OVL/hello_pad clean all
	$(MAKE) -C OVL/hello_poly clean all
	$(MAKE) -C OVL/hello_sprt clean all
	$(MAKE) -C OVL/hello_tile clean all
	mkpsxiso -y ./isoconfig.xml
cleansub:
	$(MAKE) -C OVL/hello_cubetex clean 
	$(MAKE) -C OVL/hello_light clean 
	$(MAKE) -C OVL/hello_ovl_world  clean 
	$(MAKE) -C OVL/hello_pad clean 
	$(MAKE) -C OVL/hello_poly clean 
	$(MAKE) -C OVL/hello_sprt clean 
	$(MAKE) -C OVL/hello_tile clean 
	$(MAKE) clean
	rm -f $(TARGET).cue $(TARGET).bin
	rm -f *.mcd *.frag *.lua *.vert
	
OVERLAYSCRIPT  ?= overlay.ld
OVERLAYSECTION ?= .ovly0 .ovly1 .ovly2 .ovly3 .ovly4 .ovly5 .ovly6

SRCS = hello_str.c \
src/str.c \
src/mod.c \
third_party/nugget/modplayer/modplayer.c \
HIT/SHIN1.HIT \
OVL/hello_ovl_world/hello_ovl_world.c \
OVL/hello_tile/hello_ovl_tile.c \
OVL/hello_poly/hello_ovl_poly.c \
OVL/hello_sprt/hello_ovl_sprt.c \
OVL/hello_cubetex/hello_ovl_cubetex.c \
OVL/hello_light/hello_ovl_light.c \
OVL/hello_pad/hello_ovl_pad.c \
OVL/hello_sprt/TIM/TIM16.tim \
OVL/hello_sprt/TIM/TIM8.tim \
OVL/hello_sprt/TIM/TIM4.tim \
OVL/hello_cubetex/TIM/cubetex.tim \

include common.mk 

