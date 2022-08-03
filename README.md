# Suckless Utilities version 6.3
## About 
These are my builds of suckless software such as dwm and st.
It's simple to compile these things. 

1. Install necessary tools and libraries 
```
Linux/Unix users:
- xorg (including drivers of course)
- base-devel (or build-essential/s)
- libX11(-devel or -dev)
- libXft(-devel or -dev) 
- libXinerama(-devel or -dev) 
- freetype(-devel or -dev) 
- fontconfig(-devel or -dev)
- font-awesome (for slstatus)
- terminus-font
- imlibs2(-devel or -dev)
- picom (for transparency)
- feh (optional)

Termux users:
- xorg 
- termux-X11 repo (via main Termux app)
- proot/chroot
- TigerVNC 
- VNC client
or
- XSDL client
- PulseAudio (if you like audio support)
````
2. Clone this repository 
3. Change directory to what suckless software do you want to use
4. Remove the `config.h` file, to make sure all patches are applied correctly
5. Copy `make clean install` and paste it on your terminal
6. Insert dwm, slstatus and/or st inside your `.xinitrc` using your favorite text editor (usually located in `/home/<username>/.xinitrc`)
7. Start it and done! 

## List of patches
### dwm
- [actualfullscreen](https://dwm.suckless.org/patches/actualfullscreen/) by Sönke Lambert
- [alpha](https://dwm.suckless.org/patches/alpha/) by Eon S. Jeon, et. al
- alttags by Anonymous (If you made it, please let me know) 
- [awesomebar](https://dwm.suckless.org/patches/awesomebar/) by ornx, improved by Yegor Bayev
- [centretitle](https://dwm.suckless.org/patches/centretitle/) by Felix Chen
- [centerfirstwindow](https://dwm.suckless.org/patches/center_first_window/) by ზურა დავითაშვილი, parts of the code by u/johannesthyssen on Reddit
- [fullgaps](https://dwm.suckless.org/patches/fullgaps/) by Maciej Janicki, et. al
- [gridmode](https://dwm.suckless.org/patches/gridmode/) by Alexandru E. Ungur, et. al
- [steam](https://dwm.suckless.org/patches/steam/) by Stein Bakkeby
- [taglabels + hide_vacant_tags](https://dwm.suckless.org/patches/taglabels/) by Timmy Keller
- [underlinetags](https://dwm.suckless.org/patches/underlinetags/) by Timmy Keller
- [winicon](https://dwm.suckless.org/patches/winicon/) by Adam Yuan

### st

- [alpha](https://st.suckless.org/patches/alpha/) by Eon S. Jeon (0.5 port), et. al
- [anysize](https://st.suckless.org/patches/anysize/) by Augusto Born de Oliveira 
- [autocomplete](https://st.suckless.org/patches/autocomplete/) by Wojciech Siewierski and Gaspar Vardanyan
- [clipboard](https://st.suckless.org/patches/clipboard/) by Kai Hendry, et. al
- [delkey](https://st.suckless.org/patches/delkey/) by Roberto E. Vargas Caballero, et. al
- [newterm](https://st.suckless.org/patches/newterm/) by Matías Lang, et. al
- [scrollback](https://st.suckless.org/patches/scrollback/) by Jochen Sprickerhof, et. al
- [w3m](https://st.suckless.org/patches/w3m/) by Avi Halachmi (might not work with the alpha patch) 

### dmenu

- [alpha](https://tools.suckless.org/dmenu/patches/alpha/) by Marcin Lukow
- [case-insensitive](https://tools.suckless.org/dmenu/patches/case-insensitive/) by Kim Torgersen
- [grid](https://tools.suckless.org/dmenu/patches/grid/) by Miles Alan
- [morecolor](https://tools.suckless.org/dmenu/patches/morecolor/) by Tanner Babcock

## Current bugs
- Taskbar not working properly

## Patching even further 

Patching everything is as easy as downloading the diff file, use the `patch` command and apply changes.

But, since this is a heavily patched version of everything, I wouldn't recommend patching even further unless if you know what you're doing.

## How the versioning system works

Suckless Uilities (the whole package and not the individual components) are versioned under the current version of the repo's dwm.
Even if dwm 6.4 releases, if the repo still uses dwm 6.3 for compatibility reasons, the whole package will be still be Suckless Utilities 6.3.

## Licensing
All programs are licensed under the MIT License, which sucks, and worse than GNU GPL, but hey, it's better than proprietary code!

## Special thanks 
* [Speedie](https://spdgmr.github.io) for helping me out with this and providing me with patches 
* [The suckless team](https://suckless.org) for maintaining suckless software suck less

## Special hates
* Microsoft for making Windows shittier
* gNOme
* Soydevs for making bloated software
* Big tech making tech worse
