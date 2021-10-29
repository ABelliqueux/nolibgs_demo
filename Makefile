.PHONY: all cleansub
all:
	mkpsxiso -y ./isoconfig.xml
cleansub:
	$(MAKE) clean
	rm -f hello_str.cue hello_str.bin
	rm -f *.mcd *.frag *.lua *.vert
	
TARGET = hello_str

OVERLAYSCRIPT  ?= overlay.ld
OVERLAYSECTION ?= .ovly0 .ovly1 .ovly2

SRCS = hello_str.c \
src/str.c \
src/mod.c \
third_party/nugget/modplayer/modplayer.c \
HIT/SHIN1.HIT \
OVL/hello_ovl_world/hello_ovl_world.c \
OVL/hello_tile/hello_ovl_tile.c \
OVL/hello_poly/hello_ovl_poly.c \

include common.mk 
