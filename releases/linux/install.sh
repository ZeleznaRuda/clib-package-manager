#!/bin/bash

mkdir -p "$HOME/.local/bin"


cp "_src/clibx" "$HOME/.local/bin/"
chmod +x "$HOME/.local/bin/clibx"

clibx init
clibx -v
clibx install "https://github.com/ZeleznaRuda/clibx-mainkit.git" -I -f

echo "The installation was successful. We hope you will make the best use of it. ;)"
