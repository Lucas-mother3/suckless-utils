# Suckless Utilities version 6.5
## About 
These are my builds of suckless software such as dwm and st, based on the work for flexipatch by bakkeby. This aims for much more streamlined configuration and patching than 6.3 (which becomes more complicated over time and whenever more patches are integrated). 

This was designed to save me some sanity in maintaining it as well as easily integrating requested patches, whenever it drops from the flexipatch upstream. This should be easy to hack and build, and should be as fast as the previous versions of my build.

## Included software

### X11
- dwm
- dmenu
- st
- slstatus
- tabbed
- sfm
- spmenu
- dwmblocks-async
- slock
- nsxiv
- slim

### Terminal
- dvtm
- abduco
- shmenu

### Applications by default
- firefox (browser)
- sfm/broot (file manager)
- ncmpcpp (mpd client/music player)
- dunst (notification daemon)
- jgmenu (floating stdin/stdout menu utility)
- spmenu (dmenu alternative)
- newsboat (rss/atom feed reader)

## Notes

### slim notes

To reload the preset theme (located in `/usr/share/themes/slim/suckless`), then run `.cache/wal/slim-reload` **after** running pywal. The script must also be ran as sudo.

This is, if you wanted a display manager added. The scripts also works a workaround unless if slim could handle loading themes from other dirs.

### eww notes

The weather widget is pretty barebones as of now. Report bugs if found.

### Configuration notes

The bare essentials of my dotfiles are located in the `config` directory. This includes `spmenu`, `picom`, `ncmpcppp`, `neofetch`, `broot`, `newsboat`,  and `wal`.

Newsboat config also include preset urls.

### Compatibility notes

For those who want the old version, check out `oldmain`. I don't plan on maintaining it myself since the flexipatch base means much more flexibility over codebase updates as well as new patches.

Note that there are some programs that is included here, mainly for compatibility or choice reasons. While slstatus is pretty barebones compared to dwmblocks-async, it is included on the repo if one decides not to have statuscmd, for example. This aims to be also compatible with already existing setups.

For the shutdown menu script (located in `scripts/shutdown`), `/usr/libexec/ssh/gnome-ssh-askpass` was set by default for asking password for killing X. Either change it to a more preferred ssh-askpass program or make sudo passwordless.

### Keybind notes

In the documentation for this suite, <kbd>Mod4Key</kbd> would be defined as <kbd>⊞ Win</kbd>/<kbd>⌘ Cmd</kbd>/<kbd>❖ Super</kbd>, depending on whichever keyboard do you use.

In most cases, you probably have only <kbd>⊞ Win</kbd>, but I added <kbd>⌘ Cmd</kbd> and <kbd>❖ Super</kbd> for Mac and advanced Linux/Unix users, respectively.

If one uses ChromeOS, <kbd>⊞ Win</kbd> equals to the <kbd>🔍 Search</kbd> key. But I don't know who uses X11 window managers inside ChromeOS.

For new to dwm, <kbd>MODKEY</kbd> or <kbd>Mod1Mask</kbd> is the <kbd>Alt</kbd> key. 

### spmenu notes

Additionals are spmenu configs made by myself, as well as scripts for `spmenu_run`. If you prefer dmenu, it still exists, and could be launched via <kbd>⊞ Win</kbd>/<kbd>⌘ Cmd</kbd>/<kbd>❖ Super</kbd>+<kbd>Alt</kbd>+<kbd>S</kbd>, while <kbd>⊞ Win</kbd>/<kbd>⌘ Cmd</kbd>/<kbd>❖ Super</kbd>+<kbd>S</kbd> would launch `spmenu_run -d` by default (only with .desktop entries, while  <kbd>⊞ Win</kbd>/<kbd>⌘ Cmd</kbd>/<kbd>❖ Super</kbd>+<kbd>Shift</kbd>+<kbd>S</kbd> would launch `spmenu_run` in a similar fashion to dmenu.

Some user scripts are also included, which has it's own set of dependencies. For example, `clipmenu-spmenu` needs `xsel` and `clipnotify`. These are optional, however.

```
clipmenu-spmenu dependencies:
- xsel
- clipnotify

screenshot-spmenu dependencies:
- curl
- xclip (X11)
- maim (X11)
- wayshot (Wayland)
- wl-clipboard (Wayland)
- slurp (Wayland)

wallpaper-spmenu dependencies:
- xwallpaper
```

Additionally, spmenu will not work on macOS, so use `dmenu` instead. 

## Building 
### Prerequisites 
```
Linux/Unix users:
- xorg (including drivers of course)
- base-devel (or build-essential/s)
- libX11(-devel or -dev)
- libXft(-devel or -dev) 
- libXcb(-devel or -dev)
- libXrender(-devel or -dev)
- libXinerama(-devel or -dev) 
- freetype(-devel or -dev) 
- fontconfig(-devel or -dev)
- Nerd Fonts (Hack as default, can be changed manually)
- imlibs2(-devel or -dev)
- picom (for transparency)
- feh (optional)
- pywal (for colors/wallpaper)
- slop (for riodraw)
- yajl (for ipc)
- eww (optional)
- jgmenu
- libexif(-devel or -dev) (for nsxiv)
- jq (for handling eww notifications)
- pamixer
- xcb-util(-devel or -dev)

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

To make the tabbed windows functionality to work:
- cut
- xargs
- grep
- pstree
- sed
- wmctrl
- xdotool
- xprop
- xwininfo

Refer to patches.def.h and config.mk for additional patch-related requirements.
```

### Compiling the whole thing 
1. Install necessary tools and libraries 
2. Clone this repository (`git clone --recurse-submodules`)
3. Change directory to what suckless software do you want to use
4. Remove the `config.h`, and `patches.h` files, to make sure all patches are applied correctly
5. Copy `make clean install` (or `make install-all` for nsxiv) and paste it on your terminal
6. Building the spmenu submodule included in this repo (by speedie) would strictly use meson as it's build system.
    1. For that, `cd` to the spmenu folder.
    2. Initialize setup via `meson setup build`. Pass `-Dwayland-=false` for disabling Wayland support.
    3. Run `ninja -C build` for building the binaries.
    4. Install via `meson install -C build`, and it'll prompt you if you would like to use sudo if not run as root.

7. Insert dwm, slstatus and/or st inside your `.xinitrc` using your favorite text editor (usually located in `$HOME/.xinitrc`)
    - Additionally, a script called `startdwm` located in `desktop` could be installed in `/usr/local/bin` which could be used to launch dwm on display managers, such as GDM or SDDM.
    - `startdwm` could be also used as the xinitrc script by putting it under $HOME and renaming it to `.xinitrc`.
8. Install the `dwmblocks` scripts (in `scripts/dwmblocks`) to your `$PATH`.
9. Install eww, and put the config inside `config/eww` to `$HOME/.config`. Put the scripts inside `scripts/eww` inside `$PATH`.
    - (Recommended) Make a hard link of `.cache/wal/colors.scss` to `.config/eww/colors.scss`.
10. Set up slim (optional)
    1. If you wanted to, all you have to do is setup the makefiles by making a `build` folder.
    2. Generate makefiles via `cmake`. Make sure the `PREFIX` variable is set on `/usr`
    3. Run `make` and `make install`.
    4. Set up the systemd service included, tweak if necessary (same applies with other inits but would have to deal with manual config)
11. Put `$HOME/.local/bin` in `$PATH` to seperate scripts and compiled applications, as well as to reduce clutter.
12. Start it and done! 


## Future plans
- [x] Rebase the dwm build to dwm-flexipatch (maybe under a new branch with a VM debug environment?)
- [x] Integrate barmodules if the dwm-flexipatch rewrite did happen
- [x] Version jump from 6.3 -> 6.4
- [ ] Potentially making this project into a desktop environment, when I feel it's ready to do so
- [ ] Use `spmenu-desktop-launcher` if it's mature/usable, retaining `spmenu_run` for backwards compatibility with existing scripts 
- [ ] Making a wiki for documenting functions in this build, as well as other important information about the project

## Patching even further 

Patching everything is as easy as editing the `patches.def.h` file included in the repo. Unlike 6.3, which had a complicated codebase, 6.4 aims for a much more streamlined process of patching things, unlike the previous version which would mean using `patch` and manually editing files whenever something isn't patched up properly. 

A huge thanks for bakkeby on the work for making suckless software easier to patch, meaning more people could modify and configure the code to their liking. 

## Contributing to the project

Contributions are welcome, as long as it follows the defined rules in [the CONTRIBUTING document](/CONTRIBUTING.md).

Documentations are also welcome, in fact, I do need someone who could maintain documentation for the project's inner workings. 

## How the versioning system works

Suckless Utilities (the whole package and not the individual components) are versioned under the current version of the repo's dwm.
Even if dwm(-flexipatch) 6.5 releases, if the repo still uses dwm(-flexipatch) 6.4 for compatibility reasons, the whole package will still be Suckless Utilities 6.4.

## Licensing
All programs are licensed under the MIT License, except for some submodules, which might have different licenses (for example, GPLv2).

## Screenshots
![Screenshot of the desktop](/pics/desktop.png)
![Screenshot of neofetch](/pics/neofetch.png)
![Screenshot of random screenshots](/pics/random.png)
![Screenshot of it's gaming abilities](/pics/gaming.png)
![A demo of it's pywal abilities](/pics/wal.gif)
![Screenhot of slim on a Debian test VM](/pics/login.png)
![Screenhot of tabb running](/pics/tabbed.png)

## Special thanks 
* [Speedie](https://speedie.site) for helping me out with this and providing me with patches 
* [The suckless team](https://suckless.org) for maintaining suckless software suck less
* [bakkeby](https://github.com/bakkeby) for creating dwm-flexipatch and related projects, making it possible to easily integrate patches

## Mirrors

* [GitHub](https://github.com/Lucas-mother3/suckless-utils) - Main mirror
* [GitLab](https://gitlab.com/Lucas-mother3/suckless-utils) - Secondary (and backup)
* [BitBucket](https://bitbucket.org/Lucas-mother3/suckless-utils) - Secondary backup
* [Codeberg](https://codeberg.org/Lucas-mother3/suckless-utils) - Tritary backup
* [speedie.site](https://git.speedie.site/Lucas-mother3/suckless-utils) - Mirror of gitlab
