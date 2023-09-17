#!/bin/sh
# This simple script uses dmenu to ask the user what they wanna do.

USER_OPT1=$(printf "Shutdown\nReboot\nExit\nLock\nKill X" | dmenu -b -l 5 -p "What do you wanna do?" -nb "#DC143C")

case "$USER_OPT1" in
"Shutdown") sudo shutdown now || doas shutdown now ;;
"Reboot") reboot ;;
"Exit") exit 0 ;;
"Lock") slock ;;
"Kill X") pkill Xorg || pkill X ;;
"") exit 0 ;;
esac
