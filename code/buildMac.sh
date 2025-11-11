#!/bin/bash
mkdir -p build

# detect Homebrew gmp path
GMP_PREFIX=$(brew --prefix gmp)

clang src/*.c \
    -Iinclude \
    -I${GMP_PREFIX}/include \
    -L${GMP_PREFIX}/lib \
    -lgmp \
    -Wall -Wextra -O2 \
    -o build/knapsack

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
    # ./build/knapsack
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
