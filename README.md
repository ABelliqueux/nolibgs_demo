## Nolibgs Demo disc

<a href="https://wiki.arthus.net/assets/nolibgs_demo_0.11-front.png"><img src="https://wiki.arthus.net/assets/nolibgs_demo_0.11-front.png" alt="Nolibg demo disc front" style="width:400px;"/></a>
<a href="https://wiki.arthus.net/assets/nolibgs_demo_0.11-back.png"><img src="https://wiki.arthus.net/assets/nolibgs_demo_0.11-back.png" alt="Nolibg demo disc back" style="width:400px;"/></a>

This is a sub-project of [https://github.com/ABelliqueux/nolibgs_hello_worlds](https://github.com/ABelliqueux/nolibgs_hello_worlds), which intends to demonstrate the combined use of several feats:

  * Streaming FMV from the CD ; the menu background is a looping [STR](https://github.com/ABelliqueux/nolibgs_hello_worlds/wiki/STR) file, which is the video format use by the PSX.  
  * Audio playback using [@nicolasnoble](https://github.com/nicolasnoble/)'s [modplayer](https://github.com/grumpycoders/pcsx-redux/tree/main/src/mips/modplayer) ; the music and sound effects are using a converted [MOD](https://github.com/ABelliqueux/nolibgs_hello_worlds/wiki/MOD) file.  
  * Using [overlays](https://github.com/JaberwockySeamonstah/PSXOverlayExample) to [load and execute](https://github.com/ABelliqueux/nolibgs_hello_worlds/tree/main/hello_ovl_exec) 'sub-process'.  

Separate examples for these features are available here :  

  * [https://github.com/ABelliqueux/nolibgs_hello_worlds/tree/main/hello_str](https://github.com/ABelliqueux/nolibgs_hello_worlds/tree/main/hello_str)  
  * [https://github.com/ABelliqueux/nolibgs_hello_worlds/tree/main/hello_mod](https://github.com/ABelliqueux/nolibgs_hello_worlds/tree/main/hello_mod)  
  * [https://github.com/ABelliqueux/nolibgs_hello_worlds/tree/main/hello_ovl_exec](https://github.com/ABelliqueux/nolibgs_hello_worlds/tree/main/hello_ovl_exec)  

## Pre-requisites

This project uses a nugget + PsyQ setup. Instructions for setting that up on your computer are available here : [https://github.com/ABelliqueux/nolibgs_hello_worlds/#installation](https://github.com/ABelliqueux/nolibgs_hello_worlds/#installation)  

## Cloning this repo

Clone recursively as we're using a submodule:

`git clone https://github.com/ABelliqueux/nolibgs_demo.git --recursive`  

## Compiling

You need [mkpsxiso](https://github.com/Lameguy64/mkpsxiso) in your $PATH to generate a PSX disk image.
Typing 
```bash
make
```
in a terminal will compile and generate the bin/cue files.  

Typing
```bash
make cleansub
``` 
will clean the current directory

##  Creating the disk image

```bash
mkpsxiso -y isoconfig.xml
```

### CD image

For the curious, a bin/cue is available from the release page : [https://github.com/ABelliqueux/nolibgs_demo/releases](https://github.com/ABelliqueux/nolibgs_demo/releases).

## Links, Thanks, Credits

Everything here was learnt from some more talented persons, mainly but not excluding others that hang around on the [psxdev discord](https://discord.com/channels/642647820683444236/642848627823345684)
Nicolas Noble, Lameguy64, NDR008, Jaby smoll seamonstah, danhans42, rama, sickle, paul, squaresoft74, and lot mores !

### Credits

Most graphic assets and the CD covers are homebrew, made by Schnappy (me), appart from the Timmy Burch image used in the "Hello sprt" example". The original file was found on https://southpark.fandom.com/wiki/Timmy_Burch.   

Fonts:  

02.10 Fenotype : https://www.dafont.com/fr/0210.font  
Hemi Head      : https://www.dafont.com/fr/hemi-head.font  

Original Mod file :  musix-shine , https://modarchive.org/index.php?request=view_profile&query=69141  

