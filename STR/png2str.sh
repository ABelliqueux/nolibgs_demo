#!/bin/bash

# Render blender files
if [ "$1" == "-b" ] || [ "$1" == "-a" ]; then
    echo "Rendering blender files..."
    prime-run blender29 -b ../sources/nolibgsdemo-idle_loop.blend -a --cycles-device CUDA+CPU
    prime-run blender29 -b ../sources/nolibgsdemo-trans.blend -a --cycles-device CUDA+CPU
    # unused
    # prime-run blender29 -b ../sources/nolibgsdemo-trans_alpha.blend -a --cycles-device CUDA+CPU
    # prime-run blender29 -b ../sources/nolibgsdemo-bg_only.blend -a --cycles-device CUDA+CPU
fi
# Convert PNGs to AVI
if [ "$1" == "-v" ] || [ "$1" == "-a" ]; then

    echo "Converting png to avi..."
    # idle loop
    ffmpeg -r 15 -f image2 -s 320x240 -i idle_loop/idle_loop%04d.png -vcodec rawvideo -pix_fmt bgr24 -vf scale=256:240,setsar=1:1,vflip -an -r 15 idle_loop.avi -y
    # transition
    ffmpeg -r 15 -f image2 -s 320x240 -i trans/trans%04d.png -vcodec rawvideo -pix_fmt bgr24 -vf scale=256:240,setsar=1:1,vflip -an -r 15 transition.avi -y
    
    # unused
    # upper part
    # ffmpeg -r 15 -f image2 -s 320x240 -i idle_loop/idle_loop%04d.png -vcodec rawvideo -pix_fmt bgr24 -vf scale=256:240,setsar=1:1,crop=320:144:0:0,vflip -an -r 15 idle_loop_up.avi -y
    # down part idle
    # ffmpeg -r 15 -f image2 -s 320x240 -i idle_loop/idle_loop%04d.png -vcodec rawvideo -pix_fmt bgr24 -vf scale=320:240,setsar=1:1,crop=320:96:0:144,vflip -an -r 15 idle_loop_down.avi -y
    # down part trans-aplpha
    # ffmpeg -r 15 -f image2 -s 320x240 -i trans_alpha/trans_alpha%04d.png -vcodec rawvideo -pix_fmt bgr24 -vf scale=320:240,setsar=1:1,crop=320:96:0:140,vflip -an -r 15 trans_alpha_down.avi -y
    # bg_only
    # ffmpeg -r 15 -f image2 -s 320x240 -i bg_only/bg_only%04d.png -vcodec rawvideo -pix_fmt bgr24 -vf scale=320:240,setsar=1:1,vflip -an -r 15 bg_only.avi -y

# Fix AVI files with mencoder
    # mencoder -force-avi-aspect 1.33
    rm *_mc.avi
    for vid in *.avi; 
        do mencoder $vid -ovc copy -o ${vid%%.*}_mc.avi;
    done
fi
# Convert AVI to STR
if [ "$1" == "-s" ] || [ "$1" == "-a" ]; then
    echo "Converting avi to str..."
    MC32.EXE -s avi2str.scr
fi
