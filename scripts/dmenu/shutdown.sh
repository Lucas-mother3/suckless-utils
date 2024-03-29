#!/bin/sh
# This simple script uses dmenu to ask the user what they wanna do.

USER_OPT1=$(printf "Shutdown\nReboot\nExit\nLock\nKill X" | spmenu -l 5 -g 1 -p "What do you wanna do?")


case "$USER_OPT1" in
"Shutdown") sudo shutdown now || doas shutdown now ;;
"Reboot") reboot ;;
"Exit") exit 0 ;;
"Lock") slock ;;
"Kill X") pkill Xorg || pkill X ;;
"") exit 0 ;;
esac
