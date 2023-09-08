# Suckless Utilities version 6.3
## About 
These are my builds of suckless software such as dwm and st.
It's simple to compile these things. 

Additionals are spmenu configs made by myself, as well as scripts for `spmenu_run`. If you prefer dmenu, it still exists, and could be launched via `Win/Super/Cmd+Alt+S`, while `Win/Super/Cmd+S` would launch `spmenu_run -d` by default (only with .desktop entries, while  `Win/Super/Cmd+Shift+S` would launch `spmenu_run` in a similar fashion to dmenu.

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
- Nerd Fonts (for slstatus)
- imlibs2(-devel or -dev)
- picom (for transparency)
- feh (optional)

Termux users:
- xorg 
- termux-X11 repo (via main Termux app)
- proot/chroot
- PulseAudio (if you like audio support)
- TigerVNC 
- VNC client
or
- XSDL client
or
- Termux:X11 (both apk and deb)

For spmenu:
- wayland-client(-devel or -dev, for Wayland support)
- wayland-scanner(-devel or -dev, for Wayland support)
- wayland-protocols(-devel or -dev, for Wayland support)
- xbcommon(-devel or -dev, for Wayland support)
- pango(-devel or -dev)
- cairo(-devel or -dev)
- libconfig(-devel or -dev)
- OpenSSL or libssl(-devel or -dev)
- meson
````
2. Clone this repository (`git clone --recurse-submodules`)
3. Change directory to what suckless software do you want to use
4. Remove the `config.h` file, to make sure all patches are applied correctly
5. Copy `make clean install` and paste it on your terminal
6. Building the spmenu submodule included in this repo (by speedie) would strictly use meson as it's build system.
6.1. For that, `cd` to the spmenu folder.
6.2. Initialize setup via `meson setup build`. Pass `-Dwayland-=false` for disabling Wayland support.
6.3. Run `ninja -C build` for building the binaries.
6.4. Install via `meson install -C build`, and it'll prompt you if you would like to use sudo if not run as root.
7. Insert dwm, slstatus and/or st inside your `.xinitrc` using your favorite text editor (usually located in `/home/<username>/.xinitrc`)
8. Start it and done! 

## Current bugs
- ~~Taskbar not working properly~~ (fixed in commit [e9015f2](https://github.com/Lucas-mother3/suckless-utils/commit/e9015f2d2a09ef66f1c9e188b277c89d23635195) & [7085f9](https://github.com/Lucas-mother3/suckless-utils/commit/7085f97d80fc203d6f54d0209af07007c0347880)). Thanks, [Speedie](https://speedie.gq)!
- ~~Unhiding a hidden window (using the show/hide function) and if it's the only program running, crashes dwm~~
- ~~Alt-tab crashes dwm altogther (idk man)~~

## Patching even further 

Patching everything is as easy as downloading the diff file, use the `patch` command and apply changes.

But, since this is a heavily patched version of everything, I wouldn't recommend patching even further unless if you know what you're doing.

## How the versioning system works

Suckless Uilities (the whole package and not the individual components) are versioned under the current version of the repo's dwm.
Even if dwm 6.4 releases, if the repo still uses dwm 6.3 for compatibility reasons, the whole package will be still be Suckless Utilities 6.3.

## Licensing
All programs are licensed under the MIT License, which sucks, and worse than GNU GPL, but hey, it's better than proprietary code!

## Screenshots
![Screenshot of neofetch](/pics/neofetch.png)
![Screenshot of random applications (Spotify, Space Cadet Pinball, NCSA Mosaic)](/pics/random.png)

## Special thanks 
* [Speedie](https://speedie.gq) for helping me out with this and providing me with patches 
* [The suckless team](https://suckless.org) for maintaining suckless software suck less
