# Scripts

Scripts designed for `suckless-utils` that either I personally include or written by myself.

## spmenu

- `clipmenu-spmenu` - port of clipmenu to spmenu.
- `pirokit-spmenu` - port of Pirokit (1337.to search utility) to spmenu.
- `screenshot-spmenu` - basic screenshot utility using spmenu.
- `wallpaper-spmenu` - basic wallpaper setting utility.
- `spboard` - port of `dboard` to spmenu.

## dmenu

- `dboard` - Emoji/copypasta/unicode grabber tool, just put which one then you paste it into your favorite program.
- `shutdown.sh` - Shutdown menu using dmenu.

## shmenu

- `shmenu_path` && `shmenu_run` - rough ports of the dmenu counterpart to shmenu.

## jgmenu

- `layoutmenu` - Enhanced version of the script from the `layoutmenu` patch in `dwm`. Is included by default in the `config.mk` file of dwm.
- `shutdown` - Shutdown menu using jgmenu, with few added options.

## dunst

- `notify-vol-send` - Backend responsible to the included `volume-dunst` script.
- `volume-dunst` - A volume indicator designed for dunst. Loosely based on [this script](https://gist.github.com/sebastiencs/5d7227f388d93374cebdf72e783fbd6a).

## dwmblocks-async

Aside from a few modified, it is the same scripts as the ones included in `dwmblocks-async` by default.

## eww

- `eww-album`, `eww-artist`, and `eww-music` - Scripts handling fetching music metadata to the `eww` widget in `../config/eww`.
- `messages.txt` and `quoter` - Generate random quotes to be displayed on the `eww` widget. `messages.txt` should be modified and put in `$HOME` or modify the `quoter` script.

## Ones I personally wrote
### `adelle-theme`

Bash script that handles setting themes, wallpapers and colorschemes using `pywal`. It also supports grabbing theme from online sources, albeit currently primitive.

Future plans include parsing repos of themes, allowing users to pick and choose colorschemes from any contributor in the repo, ala a package manager.

Usage:

```
h or help: Prints this text
s or set: Set the colorscheme
sw or setwall: Set the wallpaper
d or download: Download themes to the theme directory
c or clear: Clear the theme directory
l or license: View the GNU General Public License Version 3.
q or quit: Quit the program
```

### `tabb`

`tabb` is a `tabbed` handler script that only does one thing: reparent or attach any windows open (even new ones) to `tabbed`.

This is designed so that any window manager aside from `dwm` could have tabbed layouts, similar to `i3`, `sway` or `(not)ion`.

The name originates from `tabc`, which does exactly the same (with few features not implemented in `tabb`), a handler designed around bspwm. I originally designed this as a daemon of sorts, but for now I thought it's a bit immature to call it a daemon, so I called it `tabb` instead (pronounced either tab-bee or just tab).

Usage:


By default, tabb would have auto-reparenting; i.e attach all windows, except for other tabbed sessions automatically.

I designed tabb so that you could disable the behavior.

```
Flags:

-e: Enable auto-reparenting behavior, i.e  attach all windows, except for other tabbed sessions automatically.
-d: Disable auto-reparenting behavior, attach all windows, except for other tabbed sessions automatically but don't reparent new ones.
```

In  my build of `dwm-flexipatch`, <kbd>⊞ Win</kbd>/<kbd>⌘ Cmd</kbd>/<kbd>❖ Super</kbd>+<kbd>Y</kbd> would execute `tabb -e`, while <kbd>⊞ Win</kbd>/<kbd>⌘ Cmd</kbd>/<kbd>❖ Super</kbd>+<kbd>Shift</kbd>+<kbd>Y</kbd> would execute `tabb -d`.

By design, `tabb` would automatically reparent new windows to a new `tabbed` session, while leaving windows in the old `tabbed` session intact.

For it to work as intended, I personally recommend using my modified build of `tabbed-flexipatch` or `tabbed-hjc` as `tabb` lacks some functions aside from it's main purpose.
