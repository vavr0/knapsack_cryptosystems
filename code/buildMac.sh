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
