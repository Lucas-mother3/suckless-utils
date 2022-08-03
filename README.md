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
