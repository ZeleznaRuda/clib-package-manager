#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" &> /dev/null && pwd)"
PROJECT_ROOT="$(realpath "$SCRIPT_DIR/../..")"
TARGET_DIR="$HOME/.local/bin"

mkdir -p "$TARGET_DIR"

if ! cp "$PROJECT_ROOT/build/clibx" "$TARGET_DIR/"; then
    echo "failed to copy clibx to $TARGET_DIR!"
    exit 1
fi

chmod +x "$TARGET_DIR/clibx"

if ! command -v "$TARGET_DIR/clibx" &> /dev/null; then
    echo "clibx command not found after installation!"
    exit 1
fi

"$TARGET_DIR/clibx" init
"$TARGET_DIR/clibx" -v
"$TARGET_DIR/clibx" install "https://github.com/ZeleznaRuda/clibx-mainkit.git" -d -f &> /dev/null

echo -e "\e[32minstallation was successful ,ejoj\e[0m"
