#!/bin/bash

PROGRAM="$HOME/.local/bin/clibx"

if [ -f "$PROGRAM" ]; then
    rm "$PROGRAM"
    echo "The program has been successfully uninstalled. Goodbye! ;)"
else
    echo "Program not found in $PROGRAM."
fi
