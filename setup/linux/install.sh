#!/bin/bash

mkdir -p "$HOME/.local/bin"

PROGRAM_DIR="../_src"

cp "$PROGRAM_DIR/clibx" "$HOME/.local/bin/"
chmod +x "$HOME/.local/bin/clib"

echo "The installation was successful. We hope you will make the best use of it. ;)"
