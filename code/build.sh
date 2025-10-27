#!/bin/bash
mkdir -p build
gcc src/*.c -Iinclude -lgmp -Wall -Wextra -O2 -o build/knapsack

if [ $? -eq 0 ]; then
    echo "✅ Build successful!"
else
    echo "❌ Build failed."
fi

