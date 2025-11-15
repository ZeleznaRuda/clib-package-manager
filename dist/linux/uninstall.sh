#!/bin/bash
set -e

PROGRAM="$HOME/.local/bin/clibx"

if [ -f "$PROGRAM" ]; then
    rm "$PROGRAM"
    echo -e "\e[32mthe program has been successfully uninstalled, SAY GOODBYE\e[0m"
else
    echo -e "\e[31mprogram not found in $PROGRAM.\e[0m"
fi
