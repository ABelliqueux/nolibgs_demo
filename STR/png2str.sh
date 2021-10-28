#!/bin/bash

SOURCE_DIR="./sources"
RENDER_DIR="./render"

# Render blender files
if [ "$1" == "-b" ] || [ "$1" == "-a" ]; then
    echo "Rendering blender files..."

    if [ ! -d $SOURCE_DIR ]; then
        mkdir $SOURCE_DIR
    fi
    if [ ! -d $RENDER_DIR ]; then
        mkdir $RENDER_DIR
    fi

    prime-run blender29 -b "$SOURCE_DIR"/nolibgsdemo-idle_loop.blend -o "$RENDER_DIR"/idle_loop/idle_####.png -a 
    prime-run blender29 -b "$SOURCE_DIR"/nolibgsdemo-trans.blend -o "$RENDER_DIR"/trans/trans_####.png -a 
    prime-run blender29 -b "$SOURCE_DIR"/nolibgsdemo-trans_reverse.blend -o "$RENDER_DIR"/trans_reverse/trans_reverse_####.png -a
fi
# Convert PNGs to AVI
if [ "$1" == "-v" ] || [ "$1" == "-a" ]; then
    if [ ! -d $RENDER_DIR ]; then
        echo "Nothing to render !"
        break
    fi
    echo "Converting png to avi..."
    # idle loop
    ffmpeg -r 15 -f image2 -s 320x240 -i "$RENDER_DIR"/idle_loop/idle_%04d.png -vcodec rawvideo -pix_fmt bgr24 -vf scale=256:240,setsar=1:1,vflip -an -r 15 "$RENDER_DIR"/idle_loop.avi -y
    # transition
    ffmpeg -r 15 -f image2 -s 320x240 -i "$RENDER_DIR"/trans/trans_%04d.png -vcodec rawvideo -pix_fmt bgr24 -vf scale=256:240,setsar=1:1,vflip -an -r 15 "$RENDER_DIR"/transition.avi -y
    # transition
    ffmpeg -r 15 -f image2 -s 320x240 -i "$RENDER_DIR"/trans_reverse/trans_reverse_%04d.png -vcodec rawvideo -pix_fmt bgr24 -vf scale=256:240,setsar=1:1,vflip -an -r 15 "$RENDER_DIR"/transition_reverse.avi -y

# Fix AVI files with mencoder
    # mencoder -force-avi-aspect 1.33
    rm -f "$RENDER_DIR"/*_mc.avi
    for vid in "$RENDER_DIR"/*.avi; 
        #~ do echo ${$(basename $vid)%%.*}
        do 
            base=${vid##*/}
            echo ${base%%.*}
            mencoder $vid -ovc copy -o "$RENDER_DIR"/${base%%.*}_mc.avi;
    done
fi
# Convert AVI to STR
if [ "$1" == "-s" ] || [ "$1" == "-a" ]; then
    echo "Converting avi to str..."
    MC32.EXE -s avi2str.scr
fi
