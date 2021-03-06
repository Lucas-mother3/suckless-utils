# Suckless Utilities version 6.2
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
4. Copy `make clean install` and paste it on your terminal
5. Insert dwm, slstatus or st inside your `.xinitrc` using your favorite text editor (usually located in `/home/<username>/.xinitrc`)
6. Start it and done! 

## How the versioning system works

Suckless Uilities (the whole package and not the individual components) are versioned under the current version of the repo's dwm.
Even if dwm 6.3 releases, if the repo still uses dwm 6.2 for compatibility reasons, the whole package will be still be Suckless Utilities 6.2.

## Special thanks 
* [Speedie](https://spdgmr.github.io) for helping me out with this and providing me with patches 
* [The suckless team](https://suckless.org) for maintaining suckless software suck less

## Special hates
* Microsoft for making Windows shittier
* gNOme
* Soydevs for making bloated software
* Big tech making tech worse
