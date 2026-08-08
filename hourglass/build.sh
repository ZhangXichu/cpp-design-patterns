#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
OUT_DIR="$SCRIPT_DIR/libs"

mkdir -p "$OUT_DIR"

for src in "$SCRIPT_DIR/plugins"/*.cpp; do
    name="$(basename "$src" .cpp)"
    out="$OUT_DIR/$name.so"
    g++ -std=c++17 -Wall -Wextra -fPIC -shared -I"$SCRIPT_DIR/include" -o "$out" "$src"
    echo "Built $out"
done
