#!/bin/bash
mkdir -p build
gcc src/*.c -Iinclude -lgmp -Wall -Wextra -O2 -o build/knapsack

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
else
    echo "❌ Build failed."
fi

# --- Experiments (no GMP) ---
echo "=== Building experiments ==="
EXPDIR="experiments"
OUTDIR="build"

if [ -d "$EXPDIR" ]; then
  # Build each .c in experiments/ to a separate binary in build/
  for SRC in "$EXPDIR"/*.c; do
    [ -e "$SRC" ] || continue
    BIN="$OUTDIR/$(basename "${SRC%.c}")"
    echo "-> gcc $SRC -> $BIN"
    gcc -Wall -Wextra -O2 -std=c11 "$SRC" -o "$BIN" || {
      echo "❌ Failed building $SRC"; exit 1;
    }
  done
  echo "✅ Experiments built."
else
  echo "(no experiments/ directory yet)"
fi
